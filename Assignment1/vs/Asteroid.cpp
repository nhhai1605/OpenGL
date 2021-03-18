#include "Asteroid.h"

Asteroid::Asteroid(float posX, float posY)
{
	this->posX = posX;
	this->posY = posY;
	int angle = rand() % 360;
	this->asteroidDirectionRadian = angle * PI / 180;
	this->asteroidSpeed = rand() % ASTEROID_MAX_SPEED + 100;
	this->vertices = rand() % 10 + 5;
	for (int i = 0; i < vertices; i++)
	{
		offsets.push_back(rand() % 4 + 7);
	}
}

Asteroid::~Asteroid()
{

}

void Asteroid::setAsteroidPos(float x, float y)
{
	this->posX = x;
	this->posY = y;
}