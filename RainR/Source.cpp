
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "timer.h"
#include "shader_program.h"
#include "vectors.h"
#include "random.h"
#include "frame_buffer.h"
#include "render_buffer.h"
#include "texture2d.h"

using namespace std::chrono_literals;

enum class RETURN_STATES: int
{
	ERROR = -1,
	SUCCESS = 0,
};

typedef struct
{
	GLuint vertexCount;
	GLuint instanceCount;
	GLuint firstVertex;
	GLuint baseInstance;
}DrawArraysIndirectCommand;

typedef struct
{
	GLuint vertexCount;
	GLuint instanceCount;
	GLuint firstVertex;
	GLuint baseVertex;
	GLuint baseInstance;
}DrawElementsIndirectCommand;



const float WINDOW_WT = 1920;
const float WINDOW_HT = 1080;
const float QUAD_WT = 0.001;
const unsigned int NB_PARTICLES = 1024 * 1024 * 8; // 8 M
const unsigned int WORKGROUP_SIZE_X = 512;
const unsigned int WORKGROUP_SIZE_Y = 1;
const unsigned int NB_WORKGROUPS_X = NB_PARTICLES / (WORKGROUP_SIZE_X * WORKGROUP_SIZE_Y);
const unsigned int NB_WORKGROUPS_Y = 1;
const bool USE_VSYNC = false;
const bool USE_SOFT_TIMED_VSYNC = false;
const bool LOG_DT = true;

const Vec3 PARTICLE_QUAD_VERTEX_DATA[] = {
	{-QUAD_WT, -QUAD_WT, 0},
	{ QUAD_WT, -QUAD_WT, 0},
	{-QUAD_WT,  QUAD_WT, 0},
	{ QUAD_WT,  QUAD_WT, 0},
};

const Vec3 SCREEN_QUAD_VERTEX_DATA[] = {
	{-1, -1, 0},
	{ 1, -1, 0},
	{-1,  1, 0},
	{ 1,  1, 0},
};
const Vec2 SCREEN_QUAD_UV_DATA[] = {
	{0, 0},
	{1, 0},
	{0, 1},
	{1, 1},
};


const unsigned int STANDARD_QUAD_ELEMENT_DATA[] = {
	0, 1, 2, 3
};


const char* VERTEX_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/default.vert";
const char* FRAGMENT_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/default.frag";
const char* COMPUTE_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/compute.glsl";



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLFWwindow* create_window(const unsigned int wt = WINDOW_WT, const unsigned int ht = WINDOW_HT)
{
	GLFWwindow* window = glfwCreateWindow(wt, ht, "RainR", NULL, NULL);
	if(window == NULL)
	{
		throw new std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	if (!USE_VSYNC) {
		glfwSwapInterval(0);
	}
	glViewport(0, 0, wt, ht);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	return window;
}

void process_input(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

// FIXME: move away
void setup_ssbo(GLuint& ssbo, Vec4 (*rand_fn)())
{
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NB_PARTICLES * sizeof(Vec4), nullptr, GL_STATIC_DRAW);
	const GLint buff_mask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	Vec4* const points = (Vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER
		, 0
		, NB_PARTICLES * sizeof(Vec4)
		, buff_mask);

	// generate random pos.
	for (unsigned int i = 0; i < NB_PARTICLES; i++)
	{
		points[i] = rand_fn();
		points[i].w = 1;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}


void run(GLFWwindow* window)
{
	// data init
	// load shader program
	ShaderProgram shader_program;
	shader_program.mShaderPaths[GL_VERTEX_SHADER] = VERTEX_SHADER_PATH;
	shader_program.mShaderPaths[GL_FRAGMENT_SHADER] = FRAGMENT_SHADER_PATH;
	shader_program.load();
	shader_program.useProgram();
	
	// load compute shader program
	ShaderProgram compute_program;
	compute_program.mShaderPaths[GL_COMPUTE_SHADER] = COMPUTE_SHADER_PATH;
	compute_program.load();
	compute_program.useProgram();

	ShaderProgram postProcess_program;
	postProcess_program.mShaderPaths[GL_VERTEX_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/postprocess.vert";
	postProcess_program.mShaderPaths[GL_FRAGMENT_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/postprocess.frag";
	postProcess_program.load();
	postProcess_program.useProgram();


	// load particle vertex data
	GLuint particle_vao;
	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);

	// element indices
	GLuint particle_ebo;
	glGenBuffers(1, &particle_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(STANDARD_QUAD_ELEMENT_DATA),
		STANDARD_QUAD_ELEMENT_DATA, GL_STATIC_DRAW);

	// vertex
	GLuint particle_quad_buffer;
	glGenBuffers(1, &particle_quad_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particle_quad_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PARTICLE_QUAD_VERTEX_DATA), 
		PARTICLE_QUAD_VERTEX_DATA, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);
	glEnableVertexAttribArray(0);

	// colors
	GLuint particle_color_buffer;
	setup_ssbo(particle_color_buffer, Random::random_v4_normalized);
	glBindBuffer(GL_ARRAY_BUFFER, particle_color_buffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// offsets
	GLuint particle_offset_buffer;
	setup_ssbo(particle_offset_buffer, Random::random_v4);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particle_offset_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particle_offset_buffer);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), nullptr);
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);


	// draw indirect buffer
	DrawElementsIndirectCommand drawCommand = {4, NB_PARTICLES, 0, 0, 0};
	GLuint indirect_buffer;
	glGenBuffers(1, &indirect_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &drawCommand, GL_STATIC_DRAW);


	// screen quad

	// load particle vertex data
	GLuint screen_vao;
	glGenVertexArrays(1, &screen_vao);
	glBindVertexArray(screen_vao);
	// element indices
	GLuint screen_ebo;
	glGenBuffers(1, &screen_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(STANDARD_QUAD_ELEMENT_DATA),
		STANDARD_QUAD_ELEMENT_DATA, GL_STATIC_DRAW);

	// vertex
	GLuint screen_quad_buffer;
	glGenBuffers(1, &screen_quad_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, screen_quad_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_QUAD_VERTEX_DATA),
		SCREEN_QUAD_VERTEX_DATA, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);
	glEnableVertexAttribArray(0);

	// uv
	GLuint screen_uv_buffer;
	glGenBuffers(1, &screen_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, screen_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_QUAD_UV_DATA),
		SCREEN_QUAD_UV_DATA, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), nullptr);
	glEnableVertexAttribArray(1);

	///////

	Texture2D colorTexture(WINDOW_WT, WINDOW_HT, GL_LINEAR, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	RenderBuffer depthBuffer(WINDOW_WT, WINDOW_HT, GL_DEPTH24_STENCIL8);
	FrameBuffer frameBuffer;
	frameBuffer.bind();
	frameBuffer.attachTexture2D(GL_COLOR_ATTACHMENT0, colorTexture);
	frameBuffer.attachRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, depthBuffer);
	
	// FIXME: move that to FrameBuffer
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);
	
	if(!frameBuffer.checkCompleteness())
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is incomplete!" << std::endl;
		frameBuffer.unbind();
	}

	GLuint screenTextureUniformLoc = glGetUniformLocation(postProcess_program.mHandle, "screenTexture");
	glUniform1i(screenTextureUniformLoc, 0);
	///////





	Timer timer;
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	// main loop
	while(!glfwWindowShouldClose(window))
	{
		static bool unique_dt_log = true;
		timer.restart();
		
		// process input
		process_input(window);

		// update
		// compute shader update particle offsets

		compute_program.useProgram();
		glDispatchComputeGroupSizeARB(
			NB_WORKGROUPS_X, NB_WORKGROUPS_Y, 1,
			WORKGROUP_SIZE_X, WORKGROUP_SIZE_Y, 1);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //needed in this context?

		// render offscreen
		frameBuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_program.useProgram();
		glBindVertexArray(particle_vao);
		glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, nullptr);
		frameBuffer.unbind();

		// render onscreen
		glClear(GL_COLOR_BUFFER_BIT);
		postProcess_program.useProgram();
		glBindVertexArray(screen_vao);
		colorTexture.bind();
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
		

		// events and swaps
		glfwSwapBuffers(window);
		glfwPollEvents();

		// get dt
		auto elapsed = timer.stop();
		const float elapsed_ns = elapsed.count();

		// log delta time?
		if (LOG_DT)
		{
			// log every 2s.
			if (time(nullptr) % 2 == 0) {
				if (unique_dt_log) {
					std::cout << "FPS: " << 1000000000 / elapsed_ns << " ( " << elapsed_ns << "ns )" << std::endl;
				}
				unique_dt_log = false;
			}
			else
			{
				unique_dt_log = true;
			}
		}

		// sleep some time?
		if(USE_SOFT_TIMED_VSYNC)
		{
			std::this_thread::sleep_for(
				1000.0ms - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
		}
	}
}


int main(int argc, char** argv)
{
	int return_status = (int)RETURN_STATES::SUCCESS;

	try
	{
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile("church1.flt", 0);
		


		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	    GLFWwindow* window = create_window();
		glewInit();

		GLint max_group_size_x;
		GLint max_group_size_y;
		GLint max_group_size_z;
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_group_size_x);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_group_size_y);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_group_size_z);
		std::cout << "Maximum compute work group count: " << max_group_size_x
			<< ", " << max_group_size_y << ", " << max_group_size_z << std::endl;
		

		run(window);


	}
	catch(std::exception& e)
	{
		std::cout << "An error occured: " << e.what() << std::endl;
		glfwTerminate();
		return_status = (int)RETURN_STATES::ERROR;
		std::cin.get();
	}
	
	return return_status;
}