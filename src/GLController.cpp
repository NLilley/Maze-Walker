#include <iostream>
#include <fstream>
#include <Windows.h>
#include <math.h>
#include "middleware\glew\include\GL\glew.h"
#include "GLController.h"
#include "middleware\glm\glm\glm.hpp"
#include "middleware\glm\glm\gtc\matrix_transform.hpp"
#include "middleware\\soil\src\SOIL.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 colour;
};

bool GLController::checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLog,
	GLenum statusType){

		GLint status;
		objectPropertyGetterFunc(objectID, statusType, &status);

		if(status != GL_TRUE) {
			GLint infoLogLength;
			glGetShaderiv(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* buffer = new GLchar[infoLogLength];

			GLsizei bufferSize;
			glGetShaderInfoLog(objectID, infoLogLength, &bufferSize, buffer);

			MessageBoxA(NULL, buffer, "Problem!", MB_OK | MB_ICONINFORMATION);

			std::ofstream logStream;
			logStream.open("shaderLogError.txt");
			logStream << "\n" << buffer;
			logStream.close();

			delete [] buffer;

			return false;
		}

		return true;

}

bool GLController::checkShaderStatus(GLuint shaderID){

	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);

}

bool GLController::checkProgramStatus(GLuint programID){

	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);

}

std::string GLController::readShaderCode(const char* fileName) {

	std::ifstream nickInput(fileName);

	if(!nickInput.good()) {

		MessageBoxA(NULL, "File failed to load...", fileName, MB_OK | MB_ICONINFORMATION);
		exit(1);
	}

	return std::string(
		std::istreambuf_iterator<char>(nickInput),
		std::istreambuf_iterator<char>());

}

void GLController::installShaders() {


	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];

	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str(); 

	glShaderSource(vertexShaderID, 1, adapter, 0);

	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();

	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if(!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID)) {
		return;
	}


	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);


	if(!checkProgramStatus(programID)) {
		return;
	}

	//Remove the shaders which we have finished with.
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

GLvoid GLController::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

}

GLvoid GLController::sendDataToOpenGL() {

	//Verticies for a plane.
	Vertex planeVerts[] =
	{
		glm::vec3(-0.5f, -0.5f, 0.0f),	//position
		glm::vec3(0.0f, 0.0f, 1.0f),	//normal
		glm::vec3(1.0f, 0.0f, 0.0f),	//colour

		glm::vec3(0.5f, -0.5f, 0.0f),	//position
		glm::vec3(0.0f, 0.0f, 1.0f),	//normal
		glm::vec3(0.0f, 1.0f, 0.0f),	//colour

		glm::vec3(0.5f, 0.5f, 0.0f),	//position
		glm::vec3(0.0f, 0.0f, 1.0f),	//normal
		glm::vec3(0.0f, 0.0f, 1.0f),	//colour

		glm::vec3(-0.5f, 0.5f, 0.0f),	//position
		glm::vec3(0.0f, 0.0f, 1.0f),	//normal
		glm::vec3(1.0f, 1.0f, 1.0f),	//colour


	};

	GLuint vertexBufferID;

	glGenBuffers(1, &vertexBufferID);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts),
		planeVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);


	GLushort indicies[] = 
	{
		0, 1, 2, 2, 3, 0,
	};

	GLuint indexBufferID;

	glGenBuffers(1, &indexBufferID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

}

GLuint GLController::InitGL()						
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	
	glLineWidth(3.0f);
	drawLines = false;

	xNormal = glm::vec3(1.0f,0.0f,0.0f);
	yNormal = glm::vec3(0.0f,1.0f,0.0f);
	zNormal = glm::vec3(0.0f,0.0f,1.0f);

	//Setup textures for splash screen.
	splashScreenTexture = new Texture("SplashScreen.bmp");

	glGenTextures(1, &splashScreenTextureID);

	glBindTexture(GL_TEXTURE_2D, splashScreenTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, splashScreenTexture->width, splashScreenTexture->height, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, splashScreenTexture->image);

	glActiveTexture(GL_TEXTURE0);

	glGenerateMipmap(GL_TEXTURE_2D);

	//Download the vertex data to openGL for use later.
	sendDataToOpenGL();

	visionLength = worldCamera.cameraPerspectiveSettings[3];

	//Initialise the wall buffers
	glGenBuffers(1, &matrixBufferID);

	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);

	//6 walls * number of nodes * 4 bytes in a  float * 16 floats in a matrix + 3 floats for colour.
	glBufferData(GL_ARRAY_BUFFER, 
		6 * (visionLength) * (visionLength) * sizeof(float) * 19,
		NULL,
		GL_STREAM_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 19 * sizeof(float), (const GLvoid*)(sizeof(GLfloat)*16));
	glVertexAttribDivisor(1,1);

	for (int i = 0; i < 4 ; i++) 
	{
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, 19 * sizeof(float),
			(const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(2 + i, 1);
	}

	installShaders();

	return TRUE;										
}

GLvoid GLController::drawWorld(World* world)
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glm::mat4 modelMatrix = glm::lookAt(worldCamera.cameraLocation,
		(worldCamera.cameraLocation + worldCamera.viewDirection),
		worldCamera.up);

	glm::mat4 perspectiveMatrix = glm::perspective(worldCamera.cameraPerspectiveSettings.x,
		worldCamera.cameraPerspectiveSettings.y,
		worldCamera.cameraPerspectiveSettings.z,
		worldCamera.cameraPerspectiveSettings.w);


	perspectiveMatrix = perspectiveMatrix * modelMatrix;


	GLint matrixUniformLocation = glGetUniformLocation(programID, "perspectiveViewMatrix");

	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &perspectiveMatrix[0][0]);

	WorldNode* tempNode = world->currentNode;

	for(int i = 0; i < world->fieldOfView; i++)
	{
		if(tempNode->leftNode != 0) 
		{
			tempNode = tempNode->leftNode;
		}
	}

	for(int i = 0; i < world->fieldOfView; i++)
	{
		if(tempNode->backNode != 0) 
		{
			tempNode = tempNode->backNode;
		}
	}

	bool travelRight = true; // This boolean is used for searching through the nodes.


	int wallNumber = 0;
	int widthOfGrid = (world->fieldOfView * 2) + 1;
	//used to tell how many leaps we have made.
	int tracker = 0;
	

	bool scanningNodesRight = true;

	glm::mat4 wallTransformMatrix = glm::mat4(1);

	glm::mat4 rotateToLeftWall = glm::rotate(glm::mat4(1), 90.0f, yNormal);
	glm::mat4 rotateToRightWall = glm::rotate(glm::mat4(1),-90.0f, yNormal);
	glm::mat4 roateToBackWall = glm::rotate(glm::mat4(1), 180.0f, yNormal);
	glm::mat4 rotateToDownWall = glm::rotate(glm::mat4(1), -90.0f ,xNormal);
	glm::mat4 rotateToUpWall = glm::rotate(glm::mat4(1), 90.0f, xNormal);

	glm::vec3 leftOffset = glm::vec3(-0.5f,0.0f,0.0f);
	glm::vec3 rightOffset = glm::vec3(0.5f,0,0.0f);
	glm::vec3 upOffset = glm::vec3(0.0f,0.5f,0.0f);
	glm::vec3 downOffset = glm::vec3(0.0f,-0.5f,0.0f);
	glm::vec3 frontOffset = glm::vec3(0.0f,0.0f,-0.5f);
	glm::vec3 backOffset = glm::vec3(0.0f,0.0f,0.5f);


	for(int i = 0, j = widthOfGrid*widthOfGrid; i < j; i++)
	{

		//Add a roof
		wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + upOffset);
		wallTransformMatrix = wallTransformMatrix * rotateToUpWall;

		glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
		glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);

		wallNumber++;

		//Add a floor
		
		wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + downOffset);
		wallTransformMatrix = wallTransformMatrix * rotateToDownWall;

		glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
		glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);

		wallNumber++;

		
		if(tempNode->left == true)
		{
			wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + leftOffset);
			wallTransformMatrix = wallTransformMatrix * rotateToLeftWall;

			glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
			glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);

			wallNumber++;
		}

		if(tempNode->right == true)
		{
			wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + rightOffset);
			wallTransformMatrix = wallTransformMatrix * rotateToRightWall;

			glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
			glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);

			wallNumber++;
		}

		if(tempNode->back == true)
		{
			wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + backOffset);
			wallTransformMatrix = wallTransformMatrix * roateToBackWall;

			glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
			glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);
			
			wallNumber++;

		}

		if(tempNode->front == true)
		{
			wallTransformMatrix = glm::translate(glm::mat4(1), tempNode->position + frontOffset);
			//The front wall is the basis wall and therefore doesn't need to be rotated.

			glBufferSubData(GL_ARRAY_BUFFER, wallNumber * sizeof(float) * 19, sizeof(float) * 16, &wallTransformMatrix[0][0]);
			glBufferSubData(GL_ARRAY_BUFFER, (wallNumber * sizeof(float) * 19) + (16 * sizeof(float)), sizeof(float) * 3, &tempNode->colour[0]);

			wallNumber++;
		}
		

		//Move to the next node
		if(scanningNodesRight)
		{
			if(tempNode->rightNode != 0 && tracker < widthOfGrid)
			{
				tempNode = tempNode->rightNode;
				tracker++;
			} else
			{
				//Maybe dangerous
				tempNode = tempNode->frontNode;
				scanningNodesRight = false;
				tracker = 0;
			}
		} else
		{
			if(tempNode->leftNode !=0 && tracker < widthOfGrid)
			{
				tempNode= tempNode->leftNode;
				tracker++;
			} else
			{
				tempNode = tempNode->frontNode;
				scanningNodesRight = true;
				tracker = 0;
			}
		}


	}

	GLint linesUniformLocation = glGetUniformLocation(programID, "drawLines");

	
	glUniform1i(linesUniformLocation, 0);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, wallNumber);

	//If we want borders for the cubes, redraw them with borders.
	if(drawLines == true) {

	
	glUniform1i(linesUniformLocation, 1);

	glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_SHORT, 0, wallNumber);
	}
}

void GLController::drawSplashScreen()
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(0);

	//Now that we have loaded the texture, draw the splash screen.
	glBindTexture(GL_TEXTURE_2D, splashScreenTextureID);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f,1.0f);
	glVertex3f(-0.8f, -0.8f, -0.5f);
	glTexCoord2f(1.0f,1.0f);
	glVertex3f(0.8f, -0.8f, 0.5f);
	glTexCoord2f(1.0f,0.0f);
	glVertex3f(0.8f, 0.8f, -0.5f);
	glTexCoord2f(0.0f,0.0f);
	glVertex3f(-0.8f, 0.8f, -0.5f);

	glEnd();

	glUseProgram(programID);
	glEnable(GL_DEPTH_TEST);

}

void GLController::drawBots(Bot* bot)
{
	
}

GLvoid GLController::cleanup()
{
	glDeleteTextures(1, &splashScreenTextureID);
	glDeleteProgram(programID);
}
