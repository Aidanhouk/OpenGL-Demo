#pragma once

#include "lighting/Light.h"

class DirectionalLight : public Light
{
private:
	glm::vec3 m_Direction;
public:
	DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f), bool isOn = 1, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~DirectionalLight() {};

	virtual void bindToShader(Shader &shader) override;

	glm::vec3 getDirection() const { return m_Direction; }

	void setDirection(const glm::vec3 &direction) { m_Direction = direction; }
};
// moving code
//float dirLightStep{ 0.0001 };

//dirLightDirection.x += dirLightStep;
//if (dirLightDirection.x < -1.0f || dirLightDirection.x > 1.0f) {
//	dirLightStep = -dirLightStep;
//	dirLightDirection.x += dirLightStep;
//}