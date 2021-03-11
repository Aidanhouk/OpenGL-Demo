#pragma once

#include <vector>
#include "glm/glm.hpp"

class PointLight;
class Shader;
class Renderer;
class VertexArray;
class ElementBuffer;

class PointLightsControl
{
private:
	std::vector<PointLight*> m_pointLights;
	Shader *m_Shader;
	Renderer *m_Renderer;
	VertexArray *m_VAO;
	ElementBuffer *m_EBO;
public:
	PointLightsControl(Shader *shader, Renderer *renderer, VertexArray *VAO, ElementBuffer *EBO);
	~PointLightsControl();

	void addPointLight(const glm::vec3 & lightPosition);
	void bindToShader(Shader & shader);
	void switchLight(unsigned int i);
	void drawAllPntLights();

	unsigned int getLightsCount() const { return m_pointLights.size(); }
	//std::vector<PointLight*> & getPointLights() { return m_pointLights; }
	glm::vec3 & getPosition(unsigned int i);
};