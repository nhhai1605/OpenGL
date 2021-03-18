#ifndef ASTEROID
#define ASTEROID
#include "Utility.h"
class Asteroid
{
public:
	Asteroid(float posX, float posY);
	~Asteroid();
	float posX, posY, asteroidDirectionRadian, asteroidSpeed;
	std::vector <float> offsets;
	int vertices;
	void setAsteroidPos(float x, float y);

private:
};


#endif // ASTEROID
