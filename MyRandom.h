#pragma once


class MyRandom{

private:

	int seed;
	unsigned int m;
	int a;

public:


	MyRandom (int randomSeed)
	{
		seed = randomSeed;
		m = 2147483647;
		a = 62089911;
		
	}

	float getRandomReal01()
	{
		seed = (a * seed) % m;

		return (seed * 1.0f) / m ;
	}

	bool getBool(float chanceOfTrue)
	{
		if(getRandomReal01() <= chanceOfTrue)
		{
			return true;
		} else
		{
			return false;
		}
	}

};