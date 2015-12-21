#pragma once

#include <Windows.h>
#include "GLController.h"
#include "World.h"
#include "middleware\glm\glm\glm.hpp"

class MyApplication
{
public:

	GLController glController;
	World world;

	HDC			hDC;		
	HGLRC		hRC;		
	HWND		hWnd;		
	HINSTANCE	hInstance;

	bool isFullscreen;
	bool isWindowActive;
	bool isDrawSplashScreen;
	bool keys[256];

	POINT mousePosition;
	glm::vec2 mouseDelta;

	int currentTime;
	int timeDelta;

	int screenWidth;
	int screenHeight;
	int screenBPP;


	MyApplication() {
		hDC = NULL;
		hRC = NULL;
		hWnd = NULL;

		isFullscreen = false;
		isWindowActive = true;
		isDrawSplashScreen = true;
		
		mouseDelta = glm::vec2(0);

		screenWidth = 1280;
		screenHeight = 720;
		screenBPP = 32;

		timeDelta = 0;
		currentTime = GetTickCount();
	
		
	}

	
	BOOL createProgramWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag);
	void killProgramWindow();

	void keyDown(int key);
	void keyUp(int key);
	void pollInput();
};
