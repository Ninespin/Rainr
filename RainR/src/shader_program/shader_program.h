#pragma once
#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>


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
	GLuint getUniformLocation(const std::string& name);
	void setUniformF(const std::string& name, float value);
	void setUniformI(const std::string& name, int value);
	void setUniformUI(const std::string& name, unsigned int value);
	void setUniformVec2(const std::string& name, float* addr_p);
	void setUniformVec2I(const std::string& name, int* addr_p);
	void setUniformVec2UI(const std::string& name, unsigned int* addr_p);
	void setUniformVec3(const std::string& name, float* addr_p);
	void setUniformVec3I(const std::string& name, int* addr_p);
	void setUniformVec3UI(const std::string& name, unsigned int* addr_p);
	void setUniformVec4(const std::string& name, float* addr_p);
	void setUniformVec4I(const std::string& name, int* addr_p);
	void setUniformVec4UI(const std::string& name, unsigned int* addr_p);
	void setUniformMat4fv(const std::string& name, float* addr_p);
	void setUniformHandleUI(const std::string& name, GLuint64 handle);


private:
	GLuint create_shader(GLenum type, const char* source);
	static std::string read_source(const char* sourcePath);
public:
	ShaderPathMap_t mShaderPaths;
	ShaderMap_t mShaders;
	GLuint mHandle;
	std::map<std::string, GLuint> mUniformLocationCache;

};

