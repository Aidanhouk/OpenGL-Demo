#include "Light.h"

Light::Light(bool isOn, glm::vec3 &color)
	: m_IsOn{ isOn }, m_Color{ color }
{
	m_Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Diffuse = color * glm::vec3(0.5f);
	m_Ambient = m_Diffuse * glm::vec3(0.2f);
}