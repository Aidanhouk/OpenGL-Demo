#include "SpotLight.h"

#include "shader/Shader.h"
#include "camera/Camera.h"

SpotLight::SpotLight(Camera * camera, bool isOn, glm::vec3 color)
	: Light(isOn, color), m_Camera{ camera }
{
}

void SpotLight::bindToShader(Shader & shader)
{
	if (m_IsOn) {
		shader.setUniformVec3("u_SpotLight.position", m_Camera->getPosition());
		shader.setUniformVec3("u_SpotLight.direction", m_Camera->getFront());
		shader.setUniform1f("u_SpotLight.cutOff", m_CutOff);
		shader.setUniform1f("u_SpotLight.outerCutOff", m_OuterCutOff);

		shader.setUniform1f("u_SpotLight.constant", m_Constant);
		shader.setUniform1f("u_SpotLight.linear", m_Linear);
		shader.setUniform1f("u_SpotLight.quadratic", m_Quadratic);

		shader.setUniformVec3("u_SpotLight.ambient", m_Ambient);
		shader.setUniformVec3("u_SpotLight.diffuse", m_Diffuse);
		shader.setUniformVec3("u_SpotLight.specular", m_Specular);
	}
	shader.setUniform1i("u_SpotLight.isOn", m_IsOn);
}