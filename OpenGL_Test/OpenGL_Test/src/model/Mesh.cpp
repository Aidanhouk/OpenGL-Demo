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
	// Связываем соответствующие текстуры
	//unsigned int diffuseNr = 1;
	//unsigned int specularNr = 1;
	//unsigned int normalNr = 1;
	//unsigned int heightNr = 1;
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		GLCall(glActiveTexture(GL_TEXTURE1 + i)); // перед связыванием активируем нужный текстурный юнит

		//std::string number;
		std::string name = m_Textures[i].m_Type;
		//f (name == "diffuse") {
		//	//number = std::to_string(diffuseNr++);
		//
		//lse if (name == "specular") {
		//	//number = std::to_string(specularNr++);
		//
		//lse if (name == "normal") {
		//	//number = std::to_string(normalNr++);
		//
		//lse if (name == "height") {
		//	//number = std::to_string(heightNr++);
		//

		// Теперь устанавливаем сэмплер на нужный текстурный юнит
		GLCall(glUniform1i(glGetUniformLocation(shader.getRedndererID(), ("u_Material." + name).c_str()), i + 1));
		// и связываем текстуру
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_ID));
	}

	//shader.setUniform1f("u_Material.shininess", 16.0f);

	// drawing mesh
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

	// Vertices coords
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Normal)));

	// Texture coords
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_TexCoords)));

	// Tangent coords
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Tangent)));

	// Bitangent coords
	GLCall(glEnableVertexAttribArray(4));
	GLCall(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct_Vertex), (void*)offsetof(struct_Vertex, m_Bitangent)));

	GLCall(glBindVertexArray(0));
}