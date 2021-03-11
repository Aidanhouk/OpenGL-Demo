#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model/Mesh.h"

class Model
{
private:
	std::vector<struct_Texture> m_Textures_loaded;
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	bool m_GammaCorrection;
	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);

	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<struct_Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	unsigned int textureFromFile(const char *path, const std::string &directory);
public:
	Model(const std::string &path, bool gamma = false);

	void setPosition(glm::vec3 position) { m_Position = position; }
	glm::vec3 getPosition() { return m_Position; }
	glm::vec3 &getPositionAlt() { return m_Position; }

	std::vector<Mesh> &getMeshes() { return m_Meshes; }
	std::vector<struct_Texture> &getTexturesLoaded() { return m_Textures_loaded; }

	void draw(Shader &shader);
};