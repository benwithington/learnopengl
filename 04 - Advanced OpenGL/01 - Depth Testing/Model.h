#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include "Shader.h"
#include "Mesh.h"

unsigned int TextureFromFile(const char* path, const std::string directory, bool gamma = false);

class Model
{
public:
	Model(const char* path);
	void Draw(Shader& shader);
private:
	std::vector<Tex> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;


	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Tex> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

