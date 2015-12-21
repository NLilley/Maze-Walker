#include "middleware\glew\include\GL\glew.h"
#include <random>
#include <time.h>
#include <fstream>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "World.h"
#include "WorldNode.h"
#include <Windows.h>


World::World() {

	//THIS NUMBER MUST BE ODD!!!
	widthOfStartingNodeGrid = 41;
	// = widthOfStartingNodeGrib /2;
	fieldOfView = 20;

	random = new MyRandom(GetTickCount());

	numberOfNodes = 0;

	position = glm::vec3(0,0,0);

	WorldNode* tempBackNode = 0;
	WorldNode* tempLeftNode = 0;

	WorldNode* tempLeftmostNode = 0;

	/*

	This is where we initialise the World with nodes.  The four WorldNode pointers are used
	in the loop to keep track of all of the node neighbour locations as we progress.

	To facilitiate the interation below, a pointer to the leftmost node has been created.
	When the loop finishes iterating over the j's the program will use this pointer to relocate itself.

	The function fills the world from left to right and then from back to front.

	*/
	for(int i = 0; i < widthOfStartingNodeGrid; i++)
	{
		for(int j = 0; j < widthOfStartingNodeGrid; j++)
		{
			WorldNode* pCreatedNode = new WorldNode();
			numberOfNodes++;

			pCreatedNode -> position.x = j - widthOfStartingNodeGrid / 2;
			pCreatedNode -> position.y = 0;
			pCreatedNode -> position.z = (widthOfStartingNodeGrid / 2) - i; 

			//Assisgn a colour to the walls of the node

			pCreatedNode->colour.r = random->getRandomReal01();
			pCreatedNode->colour.g = random->getRandomReal01();
			pCreatedNode->colour.b = random->getRandomReal01();


			//We want the user to start in the middle of the world.
			if(i == widthOfStartingNodeGrid /2 && j == widthOfStartingNodeGrid /2)
			{
				currentNode = pCreatedNode;
			}

			if(j == 0)
			{
				if (i !=0 && j ==0)
				{
					pCreatedNode->backNode = tempLeftmostNode;
					tempLeftmostNode->frontNode = pCreatedNode;

					tempBackNode = tempLeftmostNode;
					tempLeftmostNode = pCreatedNode;

				} else 
				{
					tempLeftmostNode = pCreatedNode;
				}
			}

			//Can maybe make this loop more efficent by moving this part into the lower loop.
			if (tempLeftNode !=0 ) 
			{
				pCreatedNode->leftNode = tempLeftNode;
				tempLeftNode->rightNode = pCreatedNode;
			}

			if(tempBackNode!=0)
			{
				pCreatedNode->backNode = tempBackNode;
				tempBackNode->frontNode = pCreatedNode;
			}


			//Check if there is a node behind.  If not, random a wall value, if so match it.
			if(tempBackNode == 0) 
			{
				pCreatedNode->back = random->getBool(0.3f);
			} else 
			{
				//if the back node has a wall then the created node needs one too.
				if(tempBackNode->front)
				{
					pCreatedNode->back = true;
				} else 
				{
					pCreatedNode->back = false;
				}
			}

			if(tempLeftNode == 0) 
			{

				pCreatedNode->left = random->getBool(0.3f);
			} else 
			{

				if(tempLeftNode->right)
				{
					pCreatedNode->left = true;
				} else 
				{
					pCreatedNode->left = false;
				}
			}


			//give a random wall for the front of the created node.



			pCreatedNode->front = random->getBool(0.3f);

			pCreatedNode->right = random->getBool(0.3f);

			//Always have a ceiling and a floor.
			pCreatedNode->up = true;
			pCreatedNode->down = true;

			//Now that the values for the node we are working on have been filled, get ready
			//to move onto the next run of the function.


			tempLeftNode = pCreatedNode;

			if(pCreatedNode->backNode != 0 && pCreatedNode->backNode->rightNode != 0)
			{
				tempBackNode = pCreatedNode->backNode->rightNode;
			}

		}//end of j

		tempBackNode = 0;
		tempLeftNode = 0;

	}//end of i


	//Our one bot.
	bot = new Bot(currentNode);
}

void World::printWorldMap() {

	//Currently assumes that the user is in the middle of the map.

	//Also, it only works while the shaders are disabled.

	std::ofstream logStream;
	logStream.open("World Map.txt");
	logStream.close();

	int worldWidth = 0;
	int worldHeight = 0;

	WorldNode* tempNode = currentNode;

	while(tempNode->leftNode != 0)
	{
		worldWidth++;
		tempNode = tempNode->leftNode;
	}

	tempNode = currentNode;

	while(tempNode->backNode != 0)
	{
		worldHeight++;
		tempNode = tempNode->backNode;
	}

	tempNode = currentNode;

	for(int j = 0; j < worldHeight; j++)
	{
		tempNode = tempNode->leftNode;
	}

	for(int j = 0; j < worldHeight; j++)
	{
		tempNode = tempNode->backNode;
	}

	//Magic Numbers!
	worldWidth = worldWidth * 2;
	worldWidth += 1;

	worldHeight = worldHeight *2;
	worldHeight += 1;

	float xLength = 2.0f / worldWidth;
	float yLength = 2.0f / worldHeight;

	float xPos = -1.0f;
	float yPos = -1.0f;

	for(int i = 0; i < worldHeight; i++)
	{
		for(int j = 0; j < worldWidth; j++)
		{
			if(tempNode->left)
			{
				glBegin(GL_LINES);
				glColor3f(tempNode->colour.r,tempNode->colour.g,tempNode->colour.b);
				glVertex2f(xPos,yPos);
				glVertex2f(xPos, yPos + yLength);
				glEnd();
			}

			if(tempNode->back)
			{
				glBegin(GL_LINES);
				glColor3f(tempNode->colour.r,tempNode->colour.g,tempNode->colour.b);
				glVertex2f(xPos,yPos);
				glVertex2f(xPos + xLength, yPos);
				glEnd();
			}

			xPos += xLength;
			if(tempNode->rightNode != 0)
			{
				tempNode = tempNode->rightNode;
			}
		}//end j
		yPos += yLength;
		xPos = -1;
		//-1 to fix the problem above.  we cant go back from a null node so needed
		//to skip a transition.
		for(int k = 0; k < worldWidth - 1; k++)
		{
			tempNode = tempNode->leftNode;
		}
		tempNode = tempNode->frontNode;
	}//end i


}

void World::movePosition(glm::vec3 changeInPosition)
{

	position += changeInPosition;
	WorldNode* tempNode;

	//If we transition to a new cube, look for empty slots and place a node there.
	if(position.x > 0.5f)
	{
		currentNode = currentNode->rightNode;
		tempNode = currentNode;
		position.x -= 1.0f;

		for(int i = 0; i < fieldOfView; i++)
		{
			if(tempNode->rightNode !=0)
			{
				tempNode = tempNode->rightNode;
			} else
			{
				generateNodeBatch(tempNode,1);
				tempNode = tempNode->rightNode;
			}
		}
	} else if (position.x < -0.5f)
	{
		currentNode = currentNode->leftNode;
		tempNode = currentNode;
		position.x += 1;

		for(int i = 0; i < fieldOfView; i++)
		{
			if(tempNode->leftNode !=0)
			{
				tempNode = tempNode->leftNode;
			} else
			{
				generateNodeBatch(tempNode,-1);
				tempNode = tempNode->leftNode;
			}
		}
	}

	//same again for z
	if(position.z > 0.5f)
	{
		currentNode = currentNode->backNode;
		tempNode = currentNode;
		position.z -= 1;

		for(int i = 0; i < fieldOfView; i++)
		{
			if(tempNode->backNode !=0)
			{
				tempNode = tempNode->backNode;
			} else
			{
				generateNodeBatch(tempNode,-2);
				tempNode = tempNode->backNode;
			}
		}
	} else if (position.z < -0.5f)
	{
		currentNode = currentNode->frontNode;
		tempNode = currentNode;
		position.z += 1;

		for(int i = 0; i < fieldOfView; i++)
		{
			if(tempNode->frontNode !=0)
			{
				tempNode = tempNode->frontNode;
			} else
			{
				generateNodeBatch(tempNode,2);
				tempNode = tempNode->frontNode;
			}
		}
	}

}

//Left = -1 Right =1 Back = -2 Front = 2 Down = -3 Up = 3;
void World::generateNewNode(WorldNode* creatingNode, int side)
{

	WorldNode* newNode = new WorldNode();

	numberOfNodes++;

	//Tell the parent node about the newNode
	*creatingNode->ppSideNode(side) = newNode;

	//Tell the new node about the node creating it.
	*newNode->ppSideNode(sideOpposite(side)) = creatingNode;

	//assign the new node a position in the grid.
	if(side == -1)
	{
		newNode-> position = creatingNode->position;
		newNode->position.x -= 1;
	} else if (side == 1) 
	{
		newNode-> position = creatingNode->position;
		newNode->position.x += 1;
	} else if (side == -2)
	{
		newNode-> position = creatingNode->position;
		newNode->position.z += 1;
	} else if (side == 2)
	{
		newNode-> position = creatingNode->position;
		newNode->position.z -= 1;
	}

	//If the new node has neighbours, tell the node about it.
	if(side == -1 || side == 1) {
		if(creatingNode->backNode != 0)
		{
			if(*creatingNode->backNode->ppSideNode(side) !=0)
			{
				newNode->backNode = *creatingNode->backNode->ppSideNode(side);

				newNode->backNode->frontNode = newNode;
			}
		}

		if(creatingNode->frontNode != 0)
		{
			if(*creatingNode->frontNode->ppSideNode(side) !=0)
			{
				newNode->frontNode = *creatingNode->frontNode->ppSideNode(side);

				newNode->frontNode->backNode = newNode;
			}
		}
	} else if (side == -2 || side == 2)
	{
		if(creatingNode->rightNode != 0)
		{
			if(*creatingNode->rightNode->ppSideNode(side) !=0)
			{
				newNode->rightNode = *creatingNode->rightNode->ppSideNode(side);

				newNode->rightNode->leftNode = newNode;
			}
		}
		if(creatingNode->leftNode != 0)
		{
			if(*creatingNode->leftNode->ppSideNode(side) !=0)
			{
				newNode->leftNode = *creatingNode->leftNode->ppSideNode(side);

				newNode->leftNode->rightNode = newNode;
			}

		}
	}

	//Assign walls to the new node.
	if(newNode->frontNode !=0)
	{
		if(newNode->frontNode->back == true)
		{
			newNode->front = true;
		} else 
		{
			newNode->front = false;
		}
	} else
	{
		newNode->front = random->getBool(0.3f);
	}

	if(newNode->backNode !=0)
	{
		if(newNode->backNode->front == true)
		{
			newNode->back = true;
		} else
		{
			newNode->back = false;
		}
	} else
	{
		newNode->back = random->getBool(0.3f);
	}

	if(newNode->leftNode !=0)
	{
		if(newNode->leftNode->right == true)
		{
			newNode->left = true;
		} else
		{
			newNode->left = false;
		}
	} else
	{
		;
		newNode->left = random->getBool(0.3f);
	}

	if(newNode->rightNode !=0)
	{
		if(newNode->rightNode->left == true)
		{
			newNode->right = true;
		} else
		{
			newNode->right = false;
		}
	} else
	{
		newNode->right = random->getBool(0.3f);
	}

	//Colour the new node.

	newNode->colour.r = random->getRandomReal01();

	newNode->colour.g = random->getRandomReal01();

	newNode->colour.b = random->getRandomReal01();

}

void World::generateNodeBatch(WorldNode* creatingNode, int side)
{
	WorldNode* tempNode = creatingNode;


	//If side is left or right, work along the z axis.
	if(side == -1 || side == 1)
	{
		while(tempNode->backNode != 0)
		{
			tempNode = tempNode->backNode;
		}

		bool finishedLooping = false;

		//we will set the finished looping flag to true when we hit the end of the node array.
		while(!finishedLooping)
		{
			if(*tempNode->ppSideNode(side) == 0)
			{
				generateNewNode(tempNode, side);

			}

			tempNode = tempNode->frontNode;

			if(tempNode == 0)
			{
				finishedLooping = true;
			}
		}

	}

	//work from left to right.
	if(side == -2 || side == 2)
	{
		while(tempNode->leftNode != 0)
		{
			tempNode = tempNode->leftNode;
		}

		bool finishedLooping = false;

		//we will set the finished looping flag to true when we hit the end of the node array.
		while(!finishedLooping)
		{
			if(*tempNode->ppSideNode(side) == 0)
			{
				generateNewNode(tempNode, side);
			}

			tempNode = tempNode->rightNode;

			if(tempNode == 0)
			{
				finishedLooping = true;
			}
		}

	}


}

//Check to see if we need to spawn more nodes from the supplied position
void World::checkForEmptyNodes(WorldNode* checkPosition, int side) 
{
	WorldNode* tempNode = checkPosition;

	for(int i = 0; i < fieldOfView; i++)
	{
		if(*tempNode->ppSideNode(side) !=0)
		{
			tempNode = *tempNode->ppSideNode(side);
		} else
		{
			generateNodeBatch(tempNode,side);
			tempNode = *tempNode->ppSideNode(side);
		}
	}
}

void World::performBotAI(float timeDelta)
{
	bot->performNextMove();
	moveBot(bot,timeDelta);
}

void World::moveBot(Bot* bot, float timeDelta)
{
	glm::vec3 changeInPosition = bot->velocity * timeDelta * bot->directionOfTravel;
	glm::vec3 newPosition = bot->botPosition;

	//clamp bot movement so that huge timeDeltas cant produce huge movements.
	if(glm::length(changeInPosition) > 0.05f) 
	{
		changeInPosition = bot->directionOfTravel * 0.05f;
	}

	newPosition += changeInPosition;

	float xLocation = newPosition.x;
	float zLocation = newPosition.z;

	//modify x and y to check for crossing node boundaries.

	xLocation = xLocation - bot->botCurrentNode->position.x;
	zLocation = zLocation - bot->botCurrentNode->position.z;

	// We have moved across a node boundary to the left.
	if(xLocation < -0.5f) 
	{
		bot->botCurrentNode = bot->botCurrentNode->leftNode;
		bot->hasAlreadyTransitioned = true;

		checkForEmptyNodes(bot->botCurrentNode, -1);

	} else if (xLocation > 0.5f)
	{
		bot->botCurrentNode = bot->botCurrentNode->rightNode;
		bot->hasAlreadyTransitioned = true;

		checkForEmptyNodes(bot->botCurrentNode, 1);
	} else if (zLocation < -0.5f)
	{
		bot->botCurrentNode = bot->botCurrentNode->frontNode;
		bot->hasAlreadyTransitioned = true;

		checkForEmptyNodes(bot->botCurrentNode, 2);
	} else if (zLocation > 0.5f)
	{
		bot->botCurrentNode = bot->botCurrentNode->backNode;
		bot->hasAlreadyTransitioned = true;

		checkForEmptyNodes(bot->botCurrentNode, -2);
	}

	//If we are in a new node
	if(bot->hasAlreadyTransitioned)
	{
		float distanceFromCenter;
		distanceFromCenter = glm::length(newPosition - bot->botCurrentNode->position);
		if(distanceFromCenter <0.05f)
		{
			bot->botPosition = bot->botCurrentNode->position;
			bot->hasAlreadyTransitioned = false;
			bot->isTravelling = false;
			bot->addCurrentNodeToHistory();
		} else
		{
			bot->botPosition = newPosition;
		}
	} else
	{
		bot->botPosition = newPosition;
	}
}

//Left = -1 Right =1 Back = -2 Front = 2 Down = -3 Up = 3;
WorldNode* World::nodeOnSideOf(WorldNode* node, int side)
{

	if(side == -1)
	{
		return node->leftNode;
	}
	if(side == 1)
	{
		return node->rightNode;
	}
	if(side == -2)
	{
		return node->backNode;
	}
	if(side == 2)
	{
		return node->frontNode;
	}

}

int World::sideOpposite(int side)
{
	return (-1 * side);
}