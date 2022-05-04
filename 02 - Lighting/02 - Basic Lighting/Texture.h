#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>

#include "stb_image.h"

class Texture
{
public:
	unsigned int texID;
	Texture(std::string texturePath);
	void Bind();
};