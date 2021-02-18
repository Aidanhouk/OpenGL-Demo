#include "DirectionalLight.h"

#include "shader/Shader.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, bool isOn, glm::vec3 color)
	: Light(isOn, color), m_Direction{ direction }
{}

void DirectionalLight::bindToShader(Shader & shader)
{
	if (m_IsOn) {
		shader.setUniformVec3("u_DirLight.direction", m_Direction);

		shader.setUniformVec3("u_DirLight.ambient", m_Ambient);
		shader.setUniformVec3("u_DirLight.diffuse", m_Diffuse);
		shader.setUniformVec3("u_DirLight.specular", m_Specular);
	}
	shader.setUniform1i("u_DirLight.isOn", m_IsOn);
}