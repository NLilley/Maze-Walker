#pragma once
#include "WorldNode.h"
#include <Windows.h>

//My stack will be FILO for the sake of the program and rather than
//generic types will take world nodes.
class WorldNodeStack
{
public:

	int maxObjects;
	int currentObject;

	WorldNodeStack(int maxNoObjects) 
	{
		if(maxNoObjects > 0)
		{
		int maxObjects = maxNoObjects;
		currentObject = 0;
		} else
		{

		}
	}

};