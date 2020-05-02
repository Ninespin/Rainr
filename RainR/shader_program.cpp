#include "shader_program.h"
#include <sstream>
#include <fstream>
#include <streambuf>

const size_t ShaderProgram::ERROR_LOG_SZ = 512;

ShaderProgram::ShaderProgram()
	: mHandle( 0 )
{
}


ShaderProgram::~ShaderProgram()
{
	for (auto shaderIt = mShaders.begin(); shaderIt != mShaders.end(); ++shaderIt)
	{
		glDetachShader(mHandle, shaderIt->second);
		glDeleteShader(shaderIt->second);
	}
}

GLuint ShaderProgram::create_shader(GLenum type, const char* source)
{
	const GLuint handle = glCreateShader(type);
	glShaderSource(handle, 1, &source, nullptr);
	glCompileShader(handle);
	int success;
	char log[ERROR_LOG_SZ];
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(handle, sizeof(log), nullptr, log);
		std::stringstream ss;
		ss << "Failed to create shader: " << log << std::endl;
		throw new std::exception(ss.str().c_str());
	}
	return handle;
}

std::string ShaderProgram::read_source(const char* sourcePath)
{
	std::ifstream t(sourcePath);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}


GLuint ShaderProgram::load()
{
	mHandle = glCreateProgram();
	for(auto shaderIt = mShaderPaths.begin(); shaderIt != mShaderPaths.end(); ++shaderIt)
	{
		const GLuint shaderHandle = create_shader(shaderIt->first, read_source(shaderIt->second.c_str()).c_str());
		mShaders[shaderIt->first] = shaderHandle;
		glAttachShader(mHandle, shaderHandle);
	}
	glLinkProgram(mHandle);

	int success;
	char log[ERROR_LOG_SZ];
	glGetProgramiv(mHandle, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(mHandle, sizeof(log), NULL, log);
		std::stringstream ss;
		ss << "Failed to link shader program: " << log << std::endl;
		throw new std::exception(ss.str().c_str());
	}

	return mHandle;
}

void ShaderProgram::useProgram()
{
	glUseProgram(mHandle);
}
