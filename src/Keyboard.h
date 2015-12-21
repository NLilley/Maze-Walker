#pragma once
#include <Windows.h>

class Keyboard
{
public:
	bool keys[256];

	void keyDown(int key);
	void keyUp(int key);
	void pollInput();
};