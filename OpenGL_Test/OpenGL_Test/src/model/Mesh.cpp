#include "Mesh.h"

#include "renderer/Renderer.h"
#include "shader/Shader.h"

Mesh::Mesh(std::vector<struct_Vertex> vertices, std::vector<unsigned int> indices, std::vector<struct_Texture> textures)
	: m_Vertices{ vertices }, m_Indices{ indices }, m_Textures{ textures }
{
	setupMesh();
}

void Mesh::draw(Shader &shader)
{
	//unsigned int diffuseNr{ 1 };
	//unsigned int specularNr{ 1 };
	//unsigned int normalNr{ 1 };
	//unsigned int heightNr{ 1 };
	//for (unsigned int i = 0; i < m_Textures.size(); ++i)
	//{
	//	GLCall(glActiveTexture(GL_TEXTURE0 + i)); // активируем соответствующий текстурный юнит перед привязкой
	//	// Получаем номер текстуры (значение N в diffuse_textureN)
	//	std::string number;
	//	std::string name = m_Textures[i].m_Type;
	//	if (name == "texture_diffuse") {
	//		number = std::to_string(diffuseNr++);
	//		shader.setUniform1i("u_Material.diffuse", i);
	//	}
	//	else if (name == "texture_specular") {
	//		number = std::to_string(specularNr++);
	//		shader.setUniform1i("u_Material.specular", i);
	//	}
	//	else if (name == "texture_normal") {
	//		number = std::to_string(normalNr++);
	//		shader.setUniform1f("u_Material.normal", i);
	//	}
	//	else if (name == "texture_height") {
	//		number = std::to_string(heightNr++);
	//		shader.setUniform1f("u_Material.height", i);
	//	}
	//
	//	GLCall(glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_ID));
	//}

	GLCall(glActiveTexture(GL_TEXTURE0 + 0));
	shader.setUniform1i("u_Material.diffuse", 0);
	GLCall(glBindTexture(GL_TEXTURE_2D, m_Textures[0].m_ID));

	GLCall(glActiveTexture(GL_TEXTURE0 + 1));
	shader.setUniform1i("u_Material.specular", 1);
	GLCall(glBindTexture(GL_TEXTURE_2D, m_Textures[1].m_ID));

	//GLCall(glActiveTexture(GL_TEXTURE0 + 2));
	//shader.setUniform1i("aNormal", 2);
	//GLCall(glBindTexture(GL_TEXTURE_2D, m_Textures[2].m_ID));

	shader.setUniform1f("u_Material.shininess", 16.0f);

	GLCall(glActiveTexture(GL_TEXTURE0));

	// Отрисовываем меш
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0));
	GLCall(glBindVertexArray(0));
}

void Mesh::setupMesh()
{
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenBuffers(1, &EBO));

	GLCall(glBindVertexArray(VAO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(struct_Vertex), &m_Vertices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW));

	// Vertices coords
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)0));

	// Vertices normals
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Normal)));

	// Texture coords
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_TexCoords)));

	// Касательный вектор вершины
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Tangent));

	// Вектор бинормали вершины
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Bitangent));

	GLCall(glBindVertexArray(0));
}

/*
void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(struct_Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	// Vertices coords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)0);
	// Vertices normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Normal));
	// Texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_TexCoords));
	//glBindVertexArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
}
*/