#include "Texture.h"

Texture::Texture(std::string texturePath) {
	int width, height, nrChannels;

	//Generate an OpenGL texture
	glGenTextures(1, &texID);

	//Bind Texture to modify
	Texture::Bind();

	//Set default opengl texture wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load Image data with stbi_image.h
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	//Use loaded image data to generate the texture
	if (data)
	{
		
		GLenum imageFormat{ GL_RGB };
		if      (nrChannels == 1) imageFormat = GL_RED;
		else if (nrChannels == 3) imageFormat = GL_RGB;
		else if (nrChannels == 4) imageFormat = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

	} else {
		std::cout << "Failed to load Texture from location " << texturePath << "\n";
	}

	stbi_image_free(data);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, texID);
}