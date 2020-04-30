
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>



#include "Timer.h"

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

typedef struct
{
	float x, y;
} Vec2;

typedef struct
{
	float x, y, z;
} Vec3;

typedef struct
{
	float x, y, z, w;
} Vec4;

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

const unsigned int PARTICLE_QUAD_ELEMENT_DATA[] = {
	0, 1, 2, 3
};


const char* VERTEX_SHADER = 
"#version 450 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec4 aColor;\n"
"layout(location = 4) in vec4 aOffs;\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"	vec3 pos = aPos + aOffs.xyz;\n"
"	color = aColor;\n"
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
"}";

const char* FRAGMENT_SHADER = 
"#version 450 core\n"
"in vec4 color;\n"
"out vec4 FragColor; \n"
"\n"
"void main()\n"
"{\n"
"	FragColor = color; \n"
"}";

const char* COMPUTE_SHADER =
"#version 450 core\n"
"#extension GL_ARB_compute_shader				: enable\n"
"#extension GL_ARB_shader_storage_buffer_object : enable\n"
"#extension GL_ARB_compute_variable_group_size  : enable\n"
"layout(std140, binding = 0) buffer offs{\n"
"  vec4 offsets[];\n"
"};\n"
"layout(local_size_variable) in;\n"
"\n"
"vec4 velocity = vec4(0.001, -0.01, 0, 0);\n"
"void main(){\n"
"  uint gid = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x;\n"
"  vec4 off = offsets[ gid ];\n"
"  for(int i = 0; i < 100; i++){\n"
"    velocity.x += fract(tan(velocity.x));\n"
"    velocity.y += atan(fract(cos(velocity.x)));\n"
"  }\n"
"  off = off + velocity;\n"
"  off.x = mod(off.x + 1, 2)-1;\n"
"  off.y = mod(off.y + 1, 2)-1;\n"
"  offsets[ gid ] = off;\n"
"\n"
"}\n";


Vec2 random_v2()
{
	Vec2 v;
	v.x = ((float)rand() / (RAND_MAX)) * 2 - 1;
	v.y = ((float)rand() / (RAND_MAX)) * 2 - 1;
	return v;
}
Vec3 random_v3()
{
	Vec3 v;
	v.x = ((float)rand() / (RAND_MAX)) * 2 - 1;
	v.y = ((float)rand() / (RAND_MAX)) * 2 - 1;
	v.z = ((float)rand() / (RAND_MAX)) * 2 - 1;
	return v;
}
Vec4 random_v4()
{
	Vec4 v;
	v.x = ((float)rand() / RAND_MAX) * 2 - 1;
	v.y = ((float)rand() / RAND_MAX) * 2 - 1;
	v.z = ((float)rand() / RAND_MAX) * 2 - 1;
	v.w = ((float)rand() / RAND_MAX) * 2 - 1;
	return v;
}
Vec4 random_v4_0_1()
{
	Vec4 v;
	v.x = (float)rand() / RAND_MAX;
	v.y = (float)rand() / RAND_MAX;
	v.z = (float)rand() / RAND_MAX;
	v.w = (float)rand() / RAND_MAX;
	return v;
}

GLuint create_shader(GLenum type, const char* source)
{
	const GLuint handle = glCreateShader(type);
	glShaderSource(handle, 1, &source, nullptr);
	glCompileShader(handle);
	int success;
	char log[512];
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(handle, sizeof(log), nullptr, log);
		std::stringstream ss;
		ss << "Failed to create shader: " << log << std::endl;
		throw new std::exception(ss.str().c_str());
	}
	return handle;
}

GLuint create_compute_program()
{
	const GLuint compute_shader = create_shader(GL_COMPUTE_SHADER, COMPUTE_SHADER);
	const GLuint program = glCreateProgram();
	glAttachShader(program, compute_shader);
	glLinkProgram(program);
	glDeleteShader(compute_shader);
	int success;
	char log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		std::stringstream ss;
		ss << "Failed to link compute program: " << log << std::endl;
		throw new std::exception(ss.str().c_str());
	}

	return program;
}

GLuint create_shader_program()
{
	const GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
	const GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	int success;
	char log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		std::stringstream ss;
		ss << "Failed to link shader program: " << log << std::endl;
		throw new std::exception(ss.str().c_str());
	}

	return program;
}

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
	const GLuint shader_program = create_shader_program();
	glUseProgram(shader_program);

	// load compute shader program
	const GLuint compute_program = create_compute_program();

	// load particle vertex data
	GLuint particle_vao;
	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);

	// element indices
	GLuint particle_ebo;
	glGenBuffers(1, &particle_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PARTICLE_QUAD_ELEMENT_DATA),
		PARTICLE_QUAD_ELEMENT_DATA, GL_STATIC_DRAW);

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
	setup_ssbo(particle_color_buffer, random_v4_0_1);
	glBindBuffer(GL_ARRAY_BUFFER, particle_color_buffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// offsets
	GLuint particle_offset_buffer;
	setup_ssbo(particle_offset_buffer, random_v4);
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
		glUseProgram(compute_program);
		glDispatchComputeGroupSizeARB(
			NB_WORKGROUPS_X, NB_WORKGROUPS_Y, 1,
			WORKGROUP_SIZE_X, WORKGROUP_SIZE_Y, 1);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //needed in this context?

		// render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_program);
		glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, nullptr);\
		
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