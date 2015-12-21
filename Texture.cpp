#include "Texture.h"
#include "middleware\soil\src\SOIL.h"
#include <Windows.h>
#include <fstream>

Texture::Texture(char* filePath)
{
	image = SOIL_load_image(filePath, &width, &height, 0, SOIL_LOAD_RGB);
	if(image == 0)
	{
		MessageBoxA(NULL,"Could not load texture from disk.",filePath,0);
	} else
	{
			/*
			std::ofstream logStream;
			logStream.open("TextureLoadingLog.txt",std::ios::app);
			logStream << "\n" << image;
			logStream.close();
			*/
	}
}

void Texture::cleanup()
{
	SOIL_free_image_data(image);

}