#include "PointLight.h"

#include "glm/gtc/matrix_transform.hpp"

#include "shader/Shader.h"

PointLight::PointLight(int lightNumber, glm::vec3 position, bool isOn, glm::vec3 color)
	: m_LightNumber{ lightNumber }, Light(isOn, color), m_Position{ position }
{}

void PointLight::bindToShader(Shader & shader)
{
	std::string pntLights{ "u_PointLights[" + std::to_string(m_LightNumber) + "]." };
	if (m_IsOn) {
		shader.setUniformVec3(pntLights + "position", m_Position);

		shader.setUniform1f(pntLights + "constant", m_Constant);
		shader.setUniform1f(pntLights + "linear", m_Linear);
		shader.setUniform1f(pntLights + "quadratic", m_Quadratic);

		shader.setUniformVec3(pntLights + "ambient", m_Ambient);
		shader.setUniformVec3(pntLights + "diffuse", m_Diffuse);
		shader.setUniformVec3(pntLights + "specular", m_Specular);
	}
	shader.setUniform1i(pntLights + "isOn", m_IsOn);
}

void PointLight::drawSource(Shader &shader, glm::mat4 viewMat, glm::mat4 projMat)
{
	glm::mat4 lightModelMat{ glm::translate(glm::mat4(1.0f), m_Position) };
	lightModelMat = glm::scale(lightModelMat, glm::vec3(0.05f));

	shader.setUniformMat4f("u_Model", lightModelMat);
	shader.setUniformMat4f("u_View", viewMat);
	shader.setUniformMat4f("u_Projection", projMat);

	shader.setUniformVec3("u_LightColor", m_Color);
}