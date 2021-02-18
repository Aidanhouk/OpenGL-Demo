#pragma once

#include <vector>
#include "glm/glm.hpp"

class PointLight;
class Shader;

class PointLightsControl
{
private:
	std::vector<PointLight*> m_pointLights;
public:
	PointLightsControl() {}
	~PointLightsControl();

	void addPointLight(const glm::vec3 & lightPosition);
	void bindToShader(Shader & shader);
	void switchLight(unsigned int i);

	unsigned int getLightsCount() const { return m_pointLights.size(); }
	std::vector<PointLight*> & getPointLights() { return m_pointLights; }
	glm::vec3 & getPosition(unsigned int i);
};