#pragma once
#include "config.h"
#include "TextureResource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"


TextureResource::TextureResource() {
	texture = -1;
}

TextureResource::~TextureResource() {
	//assert(false);
	if (texture != -1) {
		glDeleteTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void TextureResource::LoadTexture(const char* name, bool rgba) {
	int width, height, channels; //Unsure if these maybe should be class members in a more general implementation?

	unsigned char* data = stbi_load(name, &width, &height, &channels, 0);

	glGenTextures(1, &this->texture);
	BindTexture(this->texture);
	if (data) {
		if(rgba)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "No data found" << "\n";
	}
	
	stbi_image_free(data);
	BindTexture(0);

	return;
}

void TextureResource::BindTexture(GLuint data) {
	glBindTexture(GL_TEXTURE_2D, data);

	return;
}
