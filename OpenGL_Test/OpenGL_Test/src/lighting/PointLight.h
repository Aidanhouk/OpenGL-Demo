#pragma once

#include "lighting/Light.h"

class PointLight : public Light
{
private:
	glm::vec3 m_Position;

	float m_Constant = 1.0f;
	float m_Linear = 0.22f;
	float m_Quadratic = 0.20f;

	int m_LightNumber;
public:
	PointLight(int lightNumber, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), bool isOn = 1, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~PointLight() {};

	virtual void bindToShader (Shader &shader) override;
	void drawSource(Shader &shader, glm::mat4 viewMat, glm::mat4 projMat);

	glm::vec3 & getPosition() { return m_Position; }

	void setPosition(const glm::vec3 &position) { m_Position = position; }
	void setConstant(float constant) { m_Constant = constant; }
	void setLinear(float linear) { m_Linear = linear; }
	void setQuadratic(float quadratic) { m_Quadratic = quadratic; }
};