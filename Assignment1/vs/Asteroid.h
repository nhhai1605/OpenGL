#ifndef ASTEROID
#define ASTEROID
#include "Utility.h"
class Asteroid
{
public:
	Asteroid(float posX, float posY, float radius, float playerX, float playerY, AsteroidState state);
	~Asteroid();
	float posX, posY, asteroidDirectionRadian, asteroidSpeed, velocityX, velocityY, mass, radius, hp;
	float rotateSpeed, asteroidRotateRadian;
	std::vector <float> offsets;
	AsteroidState state;
	int vertices;
	void setAsteroidPos(float x, float y);
	void rotate();
	void move();
	void getHit();
private:
};


#endif // ASTEROID
