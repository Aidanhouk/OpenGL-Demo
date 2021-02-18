#include "Model.h"

#include <iostream>
#include "stb_image/stb_image.h"

#include "renderer/Renderer.h"
#include "shader/Shader.h"

void Model::loadModel(const std::string &path)
{
	Assimp::Importer import;
	const aiScene *scene{ import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs) };

	ASSERT(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode));
	m_Directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// processing node's meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		m_Meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
	}
	// processing all children nodes
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<struct_Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<struct_Texture> textures;

	// processing vertex coords, normals and texture coords
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		struct_Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.m_Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.m_Normal = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.m_TexCoords = vec;
		}
		else
			vertex.m_TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// processing indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
	// processing material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<struct_Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<struct_Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. Карты нормалей
		std::vector<struct_Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 4. Карты высот
		std::vector<struct_Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<struct_Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<struct_Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip{ false };
		for (unsigned int j = 0; j < m_Textures_loaded.size(); j++) {
			if (std::strcmp(m_Textures_loaded[j].m_Path.data(), str.C_Str()) == 0) {
				textures.push_back(m_Textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			struct_Texture texture;
			texture.m_ID = textureFromFile(str.C_Str(), m_Directory);
			texture.m_Type = typeName;
			texture.m_Path = str.C_Str();
			textures.push_back(texture);
			m_Textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::textureFromFile(const char * path, const std::string & directory, bool gamma)
{
	std::string filename{ std::string(path) };
	filename = directory + '/' + filename;

	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Model::Model(const std::string &path, bool gamma)
	: m_GammaCorrection{ gamma }
{
	loadModel(path);
}

void Model::draw(Shader &shader)
{
	for (auto & mesh : m_Meshes)
		mesh.draw(shader);
}
