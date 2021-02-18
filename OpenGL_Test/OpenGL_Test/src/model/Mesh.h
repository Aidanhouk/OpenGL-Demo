#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>

class Shader;

struct struct_Vertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoords;

	//  асательный вектор
	glm::vec3 m_Tangent;
	// ¬ектор бинормали (вектор, перпендикул€рный касательному вектору и вектору нормали)
	glm::vec3 m_Bitangent;
};

struct struct_Texture
{
	unsigned int m_ID;
	std::string m_Type;
	std::string m_Path;
};

class Mesh {
public:
	std::vector<struct_Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<struct_Texture> m_Textures;

	Mesh(std::vector<struct_Vertex> vertices, std::vector<unsigned int> indices, std::vector<struct_Texture> textures);
	void draw(Shader &shader);
private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};