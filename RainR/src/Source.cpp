
#include <iostream>
#include <chrono>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "buffer/frame_buffer.h"
#include "buffer/vertex_buffer_object.h"
#include "buffer/element_buffer_object.h"
#include "camera/camera.h"
#include "mesh/mesh.h"
#include "mesh/mesh_register.h"
#include "shader_program/shader_program.h"
#include "texture/render_buffer.h"
#include "texture/texture2d.h"
#include "util/random.h"
#include "util/timer.h"
#include "util/vectors.h"
#include "buffer/vertex_array_buffer.h"
#include "buffer/shader_storage_buffer.h"
#include "material/MaterialData.h"
#include "buffer/uniform_buffer.h"
#include <sstream>


using namespace std::chrono_literals;

enum RETURN_STATES: int
{
	RETURN_STATE_ERROR = -1,
	RETURN_STATE_SUCCESS = 0,
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
const float WINDOW_ASPECT = WINDOW_WT / WINDOW_HT;
const float QUAD_WT = 1;
const unsigned int NB_PARTICLES = 1024 /** 1024*/ * 8; // 8 M
const unsigned int WORKGROUP_SIZE_X = 512;
const unsigned int WORKGROUP_SIZE_Y = 1;
const unsigned int NB_WORKGROUPS_X = NB_PARTICLES / (WORKGROUP_SIZE_X * WORKGROUP_SIZE_Y);
const unsigned int NB_WORKGROUPS_Y = 1;
const bool USE_VSYNC = false;
const bool USE_SOFT_TIMED_VSYNC = false;
const bool LOG_DT = true;
const float CAMERA_SPEED_DEFAULT = 10.0f;
const float CAMERA_SPEED_FAST = 100.0f;
const float CAMERA_SPEED_SLOW = 1.0f;
float CAMERA_SPEED = 5.0f;
const float MOUSE_SENSITIVITY = 1.0f;

Vec3 PARTICLE_QUAD_VERTEX_DATA[] = {
	{-QUAD_WT, -QUAD_WT, 0},
	{ QUAD_WT, -QUAD_WT, 0},
	{-QUAD_WT,  QUAD_WT, 0},
	{ QUAD_WT,  QUAD_WT, 0},
};

Vec3 SCREEN_QUAD_VERTEX_DATA[] = {
	{-1, -1, 0},
	{ 1, -1, 0},
	{-1,  1, 0},
	{ 1,  1, 0},
};
Vec2 SCREEN_QUAD_UV_DATA[] = {
	{0, 0},
	{1, 0},
	{0, 1},
	{1, 1},
};

unsigned int STANDARD_QUAD_ELEMENT_DATA[] = {
	0, 1, 2, 3
};

const unsigned int DEFAULT_MATERIAL_BIND_POINT = 1;


const char* VERTEX_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/default/default.vert";
const char* FRAGMENT_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/default/default.frag";
const char* COMPUTE_SHADER_PATH = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/compute.glsl";

float DELTA_TIME = 0;
float LAST_FRAME_TIME = 0;
std::map<int, bool> key_states;
Camera camera = Camera(glm::radians(45.0f), WINDOW_ASPECT, 0.1f, 1000.0f);

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::string _source, _type, _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}
	if (type == GL_DEBUG_TYPE_ERROR) {
		std::cout << "OpenGL error report: "
			<< "\n  id: " << std::hex << id << std::dec
			<< "\n  type: " << _type
			<< "\n  severity: " << _severity
			<< "\n  source: " << _source
			<< "\n  message:\n  " << message
			<< "\n\n" << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
	static float lastX = x;
	static float lastY = y;
	float dx = (x - lastX) * MOUSE_SENSITIVITY;
	float dy = (lastY - y) * MOUSE_SENSITIVITY;
	lastX = x;
	lastY = y;
	camera.rotate(dx, dy, 0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if(action == GLFW_PRESS)
	{
		key_states[key] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		key_states[key] = false;
	}
}

void process_input(GLFWwindow* window)
{
	// program termination
	if (key_states[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	// camera translation
	float cameraSpeed = CAMERA_SPEED_DEFAULT;

	if (key_states[GLFW_KEY_LEFT_SHIFT])
		cameraSpeed = CAMERA_SPEED_FAST;
	if (key_states[GLFW_KEY_LEFT_CONTROL])
		cameraSpeed = CAMERA_SPEED_SLOW;

	cameraSpeed *= DELTA_TIME;

	if (key_states[GLFW_KEY_W])
		camera.translate(cameraSpeed * camera.mForward);
	if (key_states[GLFW_KEY_S])
		camera.translate(-cameraSpeed * camera.mForward);
	if (key_states[GLFW_KEY_A])
		camera.translate(-glm::normalize(glm::cross(camera.mForward, camera.mUp)) * cameraSpeed);
	if (key_states[GLFW_KEY_D])
		camera.translate(glm::normalize(glm::cross(camera.mForward, camera.mUp)) * cameraSpeed);
	if (key_states[GLFW_KEY_Q])
		camera.translate(-cameraSpeed * camera.mUp); // FIXME: up not working properly when looking up
	if (key_states[GLFW_KEY_E])
		camera.translate(cameraSpeed * camera.mUp); // FIXME: down not working properly when looking up

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, key_callback);

	return window;
}


// FIXME: move away
void setup_ssbo(ShaderStorageBuffer<Vec4>& ssbo, Vec4 (*rand_fn)())
{
	Vec4* const points = ssbo.map();

	// generate random pos.
	for (unsigned int i = 0; i < NB_PARTICLES; i++)
	{
		points[i] = rand_fn();
		points[i].w = 1;
	}

	ssbo.unmap();
}

struct DrawEntry_t
{
	aiNode* node;
	unsigned int meshIndex;
	unsigned int modelMatrixIndex;
	unsigned int materialIndex;
	UniformBuffer<aiMatrix4x4>* modelUBO;
};
typedef std::vector<DrawEntry_t> DrawQueue_t;
typedef std::map<unsigned int, DrawQueue_t> DrawQueueMatBatch_t;
DrawQueueMatBatch_t MaterialDrawQueue;

std::string mat4_to_string(const aiMatrix4x4& matrix)
{
	std::stringstream ss;
	ss  << "matix 4x4:\n  " << matrix.a1 << " " << matrix.a2 << " " << matrix.a3 << " " << matrix.a4
		<< " \n  " << matrix.b1 << " " << matrix.b2 << " " << matrix.b3 << " " << matrix.b4
		<< " \n  " << matrix.c1 << " " << matrix.c2 << " " << matrix.c3 << " " << matrix.c4
		<< " \n  " << matrix.d1 << " " << matrix.d2 << " " << matrix.d3 << " " << matrix.d4;
	return ss.str();
}

std::string material_to_string(const MaterialData& matData)
{
	std::stringstream ss;
	ss << "material: " 
		<< "\n  diffuse: " << matData.diffuse.x << " " << matData.diffuse.y << " " << matData.diffuse.z
		<< "\n  ambient: " << matData.ambient.x << " " << matData.ambient.y << " " << matData.ambient.z
		<< "\n  specular: " << matData.specular.x << " " << matData.specular.y << " " << matData.specular.z
		<< "\n  emissive: " << matData.emissive.x << " " << matData.emissive.y << " " << matData.emissive.z
		<< "\n  opacity: " << matData.opacity
		<< "\n  shininess: " << matData.shininess
		<< "\n  shininess str: " << matData.shininessStrength;
	return ss.str();
}

std::string drawEntry_to_string(const DrawEntry_t& drawEntry)
{
	std::stringstream ss;
	ss << "draw entry: "
		<< "\n  aiNode ptr: " << std::hex << drawEntry.node << std::dec
		<< "\n  mesh index: " << drawEntry.meshIndex
		<< "\n  model matrix index: " << drawEntry.modelMatrixIndex
		<< "\n  material index: " << drawEntry.materialIndex
		<< "\n  model matrix buffer ptr: " << std::hex << drawEntry.modelUBO << std::dec;
	return ss.str();
}

void extract_materials(const aiScene& scene, UniformBuffer<MaterialData>& ubo)
{
	for(unsigned int i = 0; i < scene.mNumMaterials; i++)
	{
		aiMaterial* material = scene.mMaterials[i];
		aiColor3D diffuse, ambient, specular, emissive;
		float opacity, shininess, shininessStr;

		MaterialData matData;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		material->Get(AI_MATKEY_TRANSPARENCYFACTOR, opacity);
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStr);
		matData.diffuse = { diffuse.r, diffuse.g, diffuse.b };
		matData.ambient = { ambient.r, ambient.g, ambient.b };
		matData.specular = { specular.r, specular.g, specular.b };
		matData.emissive = { emissive.r, emissive.g, emissive.b };
		matData.opacity = opacity;
		matData.shininess = shininess;
		matData.shininessStrength = shininessStr;
		ubo.mData.push_back({ matData });
		//std::cout << material_to_string(matData) << std::endl;
		
	}
	ubo.recompile();
	std::cout << "\nMaterial collection completed, " << ubo.mData.size() << " materials found!" << std::endl;
}

void extract_model_mats(const aiScene& scene, aiNode& node, UniformBuffer<MaterialData>& materialUBO, UniformBuffer<aiMatrix4x4>& modelUBO, const aiMatrix4x4 parentMatrix = aiMatrix4x4())
{
	const aiMatrix4x4 temp = parentMatrix * node.mTransformation;
	aiMatrix4x4 currentMatrix = temp;
	currentMatrix.Transpose();
	modelUBO.mData.push_back({ currentMatrix });
	
	//std::cout << mat4_to_string(currentMatrix) << std::endl;

	// fill draw queue using material batching
	for(unsigned int i = 0; i < node.mNumMeshes; i++)
	{
		const unsigned int materialIndex = scene.mMeshes[node.mMeshes[i]]->mMaterialIndex;
		DrawEntry_t drawEntry = { &node, node.mMeshes[i], modelUBO.mData.size() - 1, materialIndex, &modelUBO };
		MaterialDrawQueue[ materialIndex ].push_back(drawEntry);
		//std::cout << drawEntry_to_string(drawEntry) << std::endl;
	}

	for (unsigned int i = 0; i < node.mNumChildren; i++)
	{
		extract_model_mats(scene, *node.mChildren[i], materialUBO, modelUBO, temp);
	}
}

void draw(DrawQueueMatBatch_t& drawQueue, UniformBuffer<MaterialData>& materialBuffer)
{
	for(const auto& materialBatch: drawQueue)
	{
		materialBuffer.bindRange(materialBatch.first);
		for(const auto& drawEntry: materialBatch.second)
		{
			drawEntry.modelUBO->bindRange(drawEntry.modelMatrixIndex);
			Mesh* mesh = MeshRegister::instance()->getMeshByIndex(drawEntry.meshIndex);//MeshRegister::instance()->getMeshByIndex(drawEntry.node->mMeshes[i]);
			mesh->draw();
		}
	}
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

	// load post-processing shader program
	ShaderProgram postProcess_program;
	postProcess_program.mShaderPaths[GL_VERTEX_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/postprocess/postprocess.vert";
	postProcess_program.mShaderPaths[GL_FRAGMENT_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/postprocess/postprocess.frag";
	postProcess_program.load();
	postProcess_program.useProgram();
	postProcess_program.setUniformUI("uScreenTexture", 0);

	// load particle shader program
	ShaderProgram particle_program;
	particle_program.mShaderPaths[GL_VERTEX_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/particle/particle.vert";
	particle_program.mShaderPaths[GL_FRAGMENT_SHADER] = "C:/Users/jeremi/source/repos/RainR/RainR/shaders/particle/particle.frag";
	particle_program.load();
	particle_program.useProgram();

	// load particle vertex data
	VertexArrayBuffer particleVao;
	ElementBufferObject<GLuint> particleEbo = ElementBufferObject<GLuint>(GL_STATIC_DRAW, 4, STANDARD_QUAD_ELEMENT_DATA);
	VertexBufferObject<Vec3> particleVerticeVbo = VertexBufferObject<Vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 3, sizeof(PARTICLE_QUAD_VERTEX_DATA), PARTICLE_QUAD_VERTEX_DATA, false, 0);

	// colors
	ShaderStorageBuffer<Vec4> particleColorSsbo = ShaderStorageBuffer<Vec4>(NB_PARTICLES, nullptr, GL_STATIC_DRAW, 0);
	setup_ssbo(particleColorSsbo, Random::random_v4_normalized);
	particleColorSsbo.bindAsVBO(3, 4, GL_FLOAT, false, 1);

	// offsets
	ShaderStorageBuffer<Vec4> particleOffsetSsbo = ShaderStorageBuffer<Vec4>(NB_PARTICLES, nullptr, GL_STATIC_DRAW, 0);
	setup_ssbo(particleOffsetSsbo, Random::random_v4);
	particleOffsetSsbo.bindAsVBO(1, 4, GL_FLOAT, false, 1);



	// draw indirect buffer
	DrawElementsIndirectCommand drawCommand = {4, NB_PARTICLES, 0, 0, 0};
	GLuint indirect_buffer;
	glGenBuffers(1, &indirect_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &drawCommand, GL_STATIC_DRAW);


	// screen quad
	VertexArrayBuffer screenVAO;
	ElementBufferObject<GLuint> screenEbo = ElementBufferObject<GLuint>(GL_STATIC_DRAW, 4, STANDARD_QUAD_ELEMENT_DATA);
	VertexBufferObject<Vec3> screenVerticesVBO = VertexBufferObject<Vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 3, sizeof(SCREEN_QUAD_VERTEX_DATA), SCREEN_QUAD_VERTEX_DATA, false, 0);
	VertexBufferObject<Vec2> screenUVVBO = VertexBufferObject<Vec2>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_FLOAT, 2, sizeof(SCREEN_QUAD_UV_DATA), SCREEN_QUAD_UV_DATA, false, 1);


	///////

	Texture2D colorTexture(WINDOW_WT, WINDOW_HT, GL_LINEAR, GL_LINEAR, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	RenderBuffer depthBuffer(WINDOW_WT, WINDOW_HT, GL_DEPTH_COMPONENT32);
	FrameBuffer frameBuffer;
	frameBuffer.bind();
	frameBuffer.attachTexture2D(GL_COLOR_ATTACHMENT0, colorTexture);
	frameBuffer.attachRenderBuffer(GL_DEPTH_ATTACHMENT, depthBuffer);
	
	// FIXME: move that to FrameBuffer
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);
	
	if(!frameBuffer.checkCompleteness())
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is incomplete!" << std::endl;
		frameBuffer.unbind();
	}

	///////
	camera.setTranslation(0, 10, 100);
	camera.setYPR(-90.0f, 0, 0);

	// Load scene
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile("C:/Users/jeremi/source/repos/RainR/Debug/car.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	MeshRegister* meshRegister = MeshRegister::instance();
	for(unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		meshRegister->registerMesh(pScene->mMeshes[i], i);
	}
	UniformBuffer<MaterialData> materialsUBO = UniformBuffer<MaterialData>(DEFAULT_MATERIAL_BIND_POINT, GL_DYNAMIC_DRAW);
	UniformBuffer<aiMatrix4x4> modelMatUBO = UniformBuffer<aiMatrix4x4>(0, GL_DYNAMIC_DRAW);
	extract_materials(*pScene, materialsUBO);
	extract_model_mats(*pScene, *pScene->mRootNode, materialsUBO, modelMatUBO);
	modelMatUBO.recompile();
	std::cout << "done recompiling model matrix ubo. size: " << modelMatUBO.mData.size() << std::endl;

	Timer timer;
	glClearColor(0.670f, 0.698f, 0.709f, 1.0f);

	glm::vec3 sun = { -100, -100, 100 };


	// main loop
	while(!glfwWindowShouldClose(window))
	{
		static bool unique_dt_log = true;
		timer.restart();
		float currentTime = glfwGetTime();
		DELTA_TIME = currentTime - LAST_FRAME_TIME;
		LAST_FRAME_TIME = currentTime;

		
		// process input
		process_input(window);

		// update


		// compute shader update particle offsets
		/*compute_program.useProgram();
		glDispatchComputeGroupSizeARB(
			NB_WORKGROUPS_X, NB_WORKGROUPS_Y, 1,
			WORKGROUP_SIZE_X, WORKGROUP_SIZE_Y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT); //needed in this context? //*/

		// render offscreen

		materialsUBO.bind();
		frameBuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_program.useProgram();
		shader_program.setUniformMat4fv("uViewProj", camera.getViewProjection());
		shader_program.setUniformVec3("uSunPos", glm::value_ptr(sun));
		
		draw(MaterialDrawQueue, materialsUBO);
		

		//particle_program.useProgram();
		//particle_program.setUniformMat4fv("uViewProj", camera.getViewProjection());
		//particleVao.bind();
		//glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, nullptr);//*/
		frameBuffer.unbind();
		materialsUBO.unbind();

		// render onscreen
		glClear(GL_DEPTH_BUFFER_BIT);
		postProcess_program.useProgram();
		screenVAO.bind();
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
			if (::time(nullptr) % 2 == 0) {
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
	int return_status = RETURN_STATE_SUCCESS;

	try
	{
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

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);


		run(window);


	}
	catch(std::exception& e)
	{
		std::cout << "An error occured: " << e.what() << std::endl;
		glfwTerminate();
		return_status = RETURN_STATE_ERROR;
		std::cin.get();
	}
	
	return return_status;
}