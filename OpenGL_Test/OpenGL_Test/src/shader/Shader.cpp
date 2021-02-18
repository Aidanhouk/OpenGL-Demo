#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "renderer/Renderer.h"

Shader::Shader(const std::string & filepath)
	: m_FilePath{ filepath }, m_RedndererID{ 0 }
{
	ShaderProgramSource source{ parseShader(filepath) };
	m_RedndererID = createShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RedndererID));
}

ShaderProgramSource Shader::parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type{ ShaderType::NONE };
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id{ glCreateShader(type) });
	const char* src{ source.c_str() };
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char *message{ (char*)alloca(length * sizeof(char)) };
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs{ compileShader(GL_VERTEX_SHADER, vertexShader) };
	unsigned int fs{ compileShader(GL_FRAGMENT_SHADER, fragmentShader) };

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

void Shader::bind() const
{
	GLCall(glUseProgram(m_RedndererID));
}

void Shader::unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string & name, int v0)
{
	GLCall(glUniform1i(getUniformLocation(name), v0));
}

void Shader::setUniform1f(const std::string & name, float v0)
{
	GLCall(glUniform1f(getUniformLocation(name), v0));
}

void Shader::setUniform3f(const std::string & name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(getUniformLocation(name), v0, v1, v2));
}

void Shader::setUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformVec3(const std::string & name, const glm::vec3 & vec3)
{
	GLCall(glUniform3f(getUniformLocation(name), vec3.x, vec3.y, vec3.z));
}

void Shader::setUniformVec4(const std::string & name, const glm::vec4 & vec4)
{
	GLCall(glUniform4f(getUniformLocation(name), vec4.x, vec4.y, vec4.z, vec4.w));
}

void Shader::setUniformMat4f(const std::string & name, const glm::mat4 & matrix)
{
	GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::getUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	GLCall(int location{ glGetUniformLocation(m_RedndererID, name.c_str()) });
	if (location == -1) {
		std::cout << "warning: uniform " << name << " doesn't exist!" << std::endl;
	}

	m_UniformLocationCache[name] = location;
	return location;
}