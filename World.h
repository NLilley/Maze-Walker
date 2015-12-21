#pragma once
#include "WorldNode.h"
#include "middleware\glm\glm\glm.hpp"
#include "MyRandom.h"
#include "Bot.h"

class World
{

private:

	glm::vec3 position;

public:
	
	World();

	int widthOfStartingNodeGrid;
	int numberOfNodes;
	int fieldOfView; //Might need to change this later.

	MyRandom* random;

	WorldNode* currentNode;
	WorldNode* nodeOnSideOf(WorldNode* node, int side);

	void movePosition(glm::vec3 newPosition);
	void generateNewNode(WorldNode* creatingNode, int side);
	void generateNodeBatch(WorldNode* creatingNode, int side);
	void checkForEmptyNodes(WorldNode* checkPosition, int side);
	
	void performBotAI(float timeDelta);
	void moveBot(Bot* bot, float timeDelta);
	Bot* bot;

	int sideOpposite(int side);

	void printWorldMap();
};