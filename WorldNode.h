#pragma once
#include "middleware\glm\glm\glm.hpp"

class WorldNode
{
public:

	/*

	This node structure cotains all of the information needed to construct the block worlds.
	The up, down, left, etc. booleans are true when there is a wall in that position and
	false when there isn't.  The front of the cube is defined to be in the 
	negative z direction, up in the poisitive y and right in the positive x.

	The node pointers are used to contain links to the other elements of the world.
	If a node has a pointer to a upNode, that means there is another world node directly
	above it.  This node may or may not be accessible from the node depending on the boolean
	values.

	If a node pointer is left empty, it means that it has yet to be generated.  Nodes should
	be created by the system when the user moves within a certain range of it (say 20 nodes away).
	In the area that the user is navigating, all nodes in every direction should be filled.

	*/

	//For the moment, we will knock up the project in 2d and then advance to 3d later.


	//True = wall, False = path clear.
	bool left;
	bool right;
	bool back;
	bool front;
	bool down;
	bool up;
	
	//WorldNode* upNode;
	//WorldNode* downNode;
	WorldNode* leftNode;
	WorldNode* rightNode;
	WorldNode* frontNode;
	WorldNode* backNode;

	glm::vec3 colour;
	glm::vec3 position;

	WorldNode()
	{
		up = false;
		down = false;
		left = false;
		right = false;
		front = false;
		back = false;

		//upNode =		0;
		//downNode =	0;
		leftNode =		0;
		rightNode =		0;
		frontNode =		0;
		backNode =		0;

		colour.r = 1.0f;
		colour.g = 1.0f;
		colour.b = 1.0f;

		position.x = 0.0f;
		position.y = 0.0f;
		position.z = 0.0f;
	}

	//Left = -1 Right =1 Back = -2 Front = 2 Down = -3 Up = 3;
	WorldNode** WorldNode::ppSideNode(int side) {
		if(side == -1) 
		{
			return &leftNode;
		}
		if(side == 1) 
		{
			return &rightNode;
		}
		if(side == -2) 
		{
			return &backNode;
		}
		if(side == 2) 
		{
			return &frontNode;
		}
		return 0;

	};

};