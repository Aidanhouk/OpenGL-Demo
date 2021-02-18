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

	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<struct_Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	unsigned int textureFromFile(const char *path, const std::string &directory, bool gamma = false);
public:
	Model(const std::string &path, bool gamma = false);
	void draw(Shader &shader);
};