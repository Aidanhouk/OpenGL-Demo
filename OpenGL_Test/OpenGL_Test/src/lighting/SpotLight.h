#pragma once

#include "lighting/Light.h"

class Camera;

class SpotLight : public Light
{
private:
	Camera *m_Camera;

	glm::vec3 m_Position;
	glm::vec3 m_Direction;
	float m_CutOff = glm::cos(glm::radians(12.5f));
	float m_OuterCutOff = glm::cos(glm::radians(17.5f));

	float m_Constant = 1.0f;
	float m_Linear = 0.22f;
	float m_Quadratic = 0.20f;
public:
	SpotLight(Camera * camera, bool isOn = 1, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~SpotLight() {};

	virtual void bindToShader(Shader &shader) override;

	glm::vec3 getPosition() const { return m_Position; }

	void setPosition(const glm::vec3 &position) { m_Position = position; }
	void setDirection(const glm::vec3 &direction) { m_Direction = direction; }
	// takes angle in degrees
	void setCutOff(float cutOff) { m_CutOff = glm::cos(glm::radians(cutOff)); }
	// takes angle in degrees
	void setOuterCutOff(float outerCutOff) { m_OuterCutOff = glm::cos(glm::radians(outerCutOff)); }
	void setConstant(float constant) { m_Constant = constant; }
	void setLinear(float linear) { m_Linear = linear; }
	void setQuadratic(float quadratic) { m_Quadratic = quadratic; }
};