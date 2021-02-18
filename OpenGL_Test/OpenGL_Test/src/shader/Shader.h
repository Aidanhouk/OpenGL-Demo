#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RedndererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string & filepath);
	~Shader();

	void bind() const;
	void unbind() const;

	// Set uniforms

	void setUniform1i(const std::string & name, int v0);
	void setUniform1f(const std::string & name, float v0);
	void setUniform3f(const std::string & name, float v0, float v1, float v2);
	void setUniform4f(const std::string & name, float v0, float v1, float v2, float v3);
	void setUniformVec3(const std::string & name, const glm::vec3 & vec3);
	void setUniformVec4(const std::string & name, const glm::vec4 & vec4);
	void setUniformMat4f(const std::string & name, const glm::mat4 & matrix);
private:
	ShaderProgramSource parseShader(const std::string& filepath);
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
	int getUniformLocation(const std::string & name);
};