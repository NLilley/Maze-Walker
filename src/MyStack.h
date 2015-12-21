#pragma once

//The MyStack object is like a simple stack.  However, if we try to add more than the maximum capacity of the stack,
//the first item in the stack (the one on the bottom) gets dropped and the new item gets added to the front of the stack.

template<class T>
class StackObject
{
public:
	T* thisObject; //A pointer to the actual object the stack object is tracking.
	StackObject<T> * nextObject;
	StackObject<T> * previousObject;

	StackObject(T* object) 
	{
		thisObject = object;
		nextObject = 0;
		previousObject = 0;
	}
};


template<class T>
class MyStack
{
public:
	int maxObjects; //Set to 0 when there is no limit to the stack.
	int numberOfObjects;

	StackObject<T> * pCurrentObject; //A pointer to the current object.
	StackObject<T> * pFirstObject;	//A pointer to the very first object added to the stack.


	MyStack(int maxNoObjects) 
	{
		if(maxNoObjects > 0)
		{
			maxObjects = maxNoObjects;

			numberOfObjects = 0;
			pCurrentObject = 0;
			pFirstObject = 0;
		} else
		{
			maxObjects = 0;
			numberOfObjects = 0;
			pCurrentObject = 0;
			pFirstObject = 0;
		}
	}

	MyStack()
	{
		maxObject = 0;
		numberOfObjects = 0;
		pCurrentObject = 0;
		pFirstObject = 0;
	}

	void addObject(T* pObject)
	{
		if(pObject != 0) { //Make sure we are not given a null pointer
			if(numberOfObjects < maxObjects || maxObjects == 0) {
				if(numberOfObjects == 0)
				{


					pCurrentObject = new StackObject<T> (pObject); // A pointer to a stack object which contains type T's.
					pFirstObject = pCurrentObject;
					numberOfObjects++;
				} else {
					StackObject<T> * tempStackObject = pCurrentObject;
					pCurrentObject = new StackObject<T> (pObject);
					tempStackObject->nextObject = pCurrentObject;
					pCurrentObject->previousObject = tempStackObject;
					numberOfObjects++;
				}
			} else //If we are at the max number of objects. 
			{
				StackObject<T> * tempStackObject = pCurrentObject;
				pCurrentObject = new StackObject<T> (pObject);
				tempStackObject->nextObject = pCurrentObject;
				pCurrentObject->previousObject = tempStackObject;

				//Now drop 1 item from the bottom of the stack.

				StackObject<T> * bottomObject = pFirstObject;
				pFirstObject = pFirstObject->nextObject;	
				numberOfObjects++;
			}
		}
	}

	T* popObject() 
	{
		StackObject<T> * tempObject = pCurrentObject;
		pCurrentObject = pCurrentObject->previousObject;
		pCurrentObject->nextObject = 0; //Set the pointer to next object to null as it is now at the front of the stack.
		numberOfObjects--;

		return tempObject->thisObject;

	}

	//Returns a pointer to the object at the top of the stack.
	T* peekTopObject()
	{
		return pCurrentObject->thisObject;
	}

	T* peekObjectFromTop(int distanceFromTop)
	{
		if(distanceFromTop < numberOfObjects)
		{
			StackObject<T>* tempObject = pCurrentObject;
			
			for(int i = 0; i < distanceFromTop; i++)
			{
				tempObject = tempObject->previousObject;
			}

			return tempObject->thisObject;
		} else
		{
			return 0;
		}
	}
};

