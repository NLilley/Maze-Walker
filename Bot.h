#pragma once
#include "middleware\glm\glm\glm.hpp"
#include "WorldNode.h"
#include "MyStack.h"

class Bot {
public:
	bool isTravelling;//If this is true then we don't need to perform any more logic about direction of travel.
	bool hasAlreadyTransitioned; //montiors if the bot has crossed a node boundary.

	float velocity;
	glm::vec3 botPosition;
	glm::vec3 directionOfTravel;

	WorldNode* botCurrentNode;
	MyStack<WorldNode>* pNodeHistory;

	Bot(WorldNode* startingNode)
	{
		pNodeHistory = new MyStack<WorldNode>(12);  //establish a stack object to track the previous cells of the bot.
		botCurrentNode = startingNode;
		pNodeHistory->addObject(startingNode);

		botPosition = startingNode->position;
		isTravelling = false;
		hasAlreadyTransitioned = false;
		velocity = 0.001f;
	}

	
	void performNextMove()
	{
		if(isTravelling == false)
		{
			if(pNodeHistory->numberOfObjects > 1 && pNodeHistory->peekTopObject() != pNodeHistory->peekObjectFromTop(2))
			{
				if(!botCurrentNode->left)
				{
					directionOfTravel = glm::vec3(-1.0f,0.0f,0.0f);
					isTravelling = true;
				} else if (!botCurrentNode->back)
				{
					directionOfTravel = glm::vec3(0.0f,0.0f,1.0f);
					isTravelling = true;
				} else if (!botCurrentNode->right)
				{
					directionOfTravel = glm::vec3(1.0f,0.0f,0.0f);
					isTravelling = true;
				} else if (!botCurrentNode->front)
				{
					directionOfTravel = glm::vec3(0.0f,-0.0f,-1.0f);
					isTravelling = true;
				}
			} else
			{
				if(!botCurrentNode->front)
				{
					directionOfTravel = glm::vec3(0.0f,0.0f,-1.0f);
					isTravelling = true;
				} else if (!botCurrentNode->right)
				{
					directionOfTravel = glm::vec3(1.0f,0.0f,0.0f);
					isTravelling = true;
				} else if (!botCurrentNode->back)
				{
					directionOfTravel = glm::vec3(0.0f,0.0f,1.0f);
					isTravelling = true;
				} else if (!botCurrentNode->left)
				{
					directionOfTravel = glm::vec3(-1.0f,-0.0f,0.0f);
					isTravelling = true;
				}
			}
		}
	}

	void addCurrentNodeToHistory()
	{
		pNodeHistory->addObject(botCurrentNode);
	};
};