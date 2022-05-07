#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Shader.h"

struct Vert {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Tex {
	unsigned int id;
	std::string type;
};


class Mesh
{
public:
	std::vector<Vert>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Tex>			textures;

	Mesh(std::vector<Vert> vertices, std::vector<unsigned int> indices, std::vector<Tex> textures);
	void Draw(Shader& shader);
private:
	unsigned int VAO, VBO, EBO;
	void setUpMesh();
};

