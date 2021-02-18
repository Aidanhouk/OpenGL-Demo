#pragma once

#include "glm/glm.hpp"

class Shader;

class Light
{
protected:
	bool m_IsOn;

	glm::vec3 m_Color;
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;
public:
	Light(bool isOn, glm::vec3 &color);
	virtual ~Light() {};

	virtual void bindToShader (Shader &shader) = 0;
	void switchLight() { m_IsOn = !m_IsOn; }

	bool getIsOn() const { return m_IsOn; }
	glm::vec3 getColor() const { return m_Color; }
	glm::vec3 getAmbient() const { return m_Ambient; }
	glm::vec3 getDiffuse() const { return m_Diffuse; }
	glm::vec3 getSpecular() const { return m_Specular; }

	void setColor(const glm::vec3 &color) { m_Color = color; }
	void setAmbient(const glm::vec3 &ambient) { m_Color = ambient; }
	void setDiffuse(const glm::vec3 &diffuse) { m_Color = diffuse; }
	void setSpecular(const glm::vec3 &specular) { m_Color = specular; }
};