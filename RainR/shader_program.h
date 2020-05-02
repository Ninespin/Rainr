#pragma once
#include <string>
#include <map>
#include <GL/glew.h>


class ShaderProgram
{
	typedef std::map<GLenum, std::string> ShaderPathMap_t;
	typedef std::map<GLenum, GLuint> ShaderMap_t;
public:
	static const size_t ERROR_LOG_SZ;

public:
	ShaderProgram();
	~ShaderProgram();
	GLuint load();
	void useProgram();

private:
	GLuint create_shader(GLenum type, const char* source);
	static std::string read_source(const char* sourcePath);
public:
	ShaderPathMap_t mShaderPaths;
	ShaderMap_t mShaders;
	GLuint mHandle;

};

