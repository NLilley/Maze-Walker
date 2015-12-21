#pragma once
#define PI = 3.14159265359

#include <iostream>
#include "middleware\glew\include\GL\glew.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include "World.h"
#include "Camera.h"
#include "Texture.h"
#include "Bot.h"

class GLController
{
public:

	glm::vec3 xNormal;
	glm::vec3 yNormal;
	glm::vec3 zNormal;

	GLuint programID;
	GLuint matrixBufferID;
	GLuint splashScreenTextureID;

	int visionLength;
	bool drawLines;

	Texture* splashScreenTexture;
	Camera worldCamera;

	

	bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLog,
	GLenum statusType);

	bool checkShaderStatus(GLuint shaderID);

	bool checkProgramStatus(GLuint programID);

	std::string readShaderCode(const char* fileName);

	void installShaders();

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

	GLuint InitGL(GLvoid);

	GLvoid sendDataToOpenGL();

	GLvoid drawWorld(World* world);

	GLvoid drawSplashScreen();

	GLvoid drawBots(Bot* bot);

	GLvoid cleanup();

};