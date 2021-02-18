#include "PointLightsControl.h"

#include "lighting/PointLight.h"

PointLightsControl::~PointLightsControl()
{
	for (const auto & pntLight : m_pointLights) {
		delete pntLight;
	}
}

void PointLightsControl::addPointLight(const glm::vec3 & lightPosition)
{
	m_pointLights.push_back(new PointLight(m_pointLights.size(), lightPosition));
}

void PointLightsControl::bindToShader(Shader & shader)
{
	for (const auto & pntLight : m_pointLights) {
		pntLight->bindToShader(shader);
	}
}

void PointLightsControl::switchLight(unsigned int i)
{
	m_pointLights[i]->switchLight();
}

glm::vec3 & PointLightsControl::getPosition(unsigned int i)
{
	return m_pointLights[i]->getPosition();
}
