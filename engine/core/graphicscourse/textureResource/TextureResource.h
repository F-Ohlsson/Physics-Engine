#pragma once
#include "config.h"
#include "core/app.h"
#include <iostream>

class TextureResource {
public:
	TextureResource();
	~TextureResource();

	GLuint texture;

	void LoadTexture(const char* name, bool rgba);
	void BindTexture(GLuint data);
};