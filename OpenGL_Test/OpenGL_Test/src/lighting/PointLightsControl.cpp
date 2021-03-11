#include "PointLightsControl.h"

#include "lighting/PointLight.h"
#include "renderer/Renderer.h"

PointLightsControl::PointLightsControl(Shader *shader, Renderer *renderer, VertexArray *VAO, ElementBuffer *EBO)
	: m_Shader{ shader }, m_Renderer{ renderer }, m_VAO{ VAO }, m_EBO{ EBO }
{
}

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

void PointLightsControl::drawAllPntLights()
{
	for (const auto & pntLight : m_pointLights) {
		if (pntLight->getIsOn()) {
			pntLight->drawSource(*m_Shader);
			m_Renderer->draw(*m_VAO, *m_EBO);
		}
	}
}

glm::vec3 & PointLightsControl::getPosition(unsigned int i)
{
	return m_pointLights[i]->getPosition();
}