#pragma once

class Texture
{
public:

	int width;
	int height;
	unsigned char* image;

	Texture(char* filePath);
	void cleanup();
};