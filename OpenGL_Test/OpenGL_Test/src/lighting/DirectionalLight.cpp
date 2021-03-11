#include "DirectionalLight.h"

#include "shader/Shader.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, bool isOn, glm::vec3 color)
	: Light(isOn, color), m_Direction{ direction }
{}

void DirectionalLight::bindToShader(Shader & shader)
{
	shader.setUniformVec3("u_DirLight.direction", m_Direction);

	shader.setUniformVec3("u_DirLight.ambient", m_Ambient);
	shader.setUniformVec3("u_DirLight.diffuse", m_Diffuse);
	shader.setUniformVec3("u_DirLight.specular", m_Specular);
	/*
	if (m_IsOn) {
		shader.setUniformVec3("u_DirLight.direction", m_Direction);

		shader.setUniformVec3("u_DirLight.ambient", m_Ambient);
		shader.setUniformVec3("u_DirLight.diffuse", m_Diffuse);
		shader.setUniformVec3("u_DirLight.specular", m_Specular);
	}
	shader.setUniform1i("u_DirLight.isOn", m_IsOn);
	*/
}

const float dirLightStep{ 0.5 };

void DirectionalLight::move(float deltatime)
{
	m_Direction.x += dirLightStep * deltatime;
	if (m_Direction.x > 30.0f) {
		m_Direction.x = -30.0f;
	}
}