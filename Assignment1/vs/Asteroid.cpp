#include "Asteroid.h"

Asteroid::Asteroid(float posX, float posY, float radius, float playerX, float playerY, AsteroidState state)
{
	this->posX = posX;
	this->posY = posY;
	this->radius = radius;
	this->mass = radius;
	this->state = state;
	this->hp = ceil(radius / 20) * 10;
	float distance = sqrt((posX - playerX) * (posX - playerX) + (posY - playerY) * (posY - playerY));
	if (posY < playerY)
	{
		if (posX > playerX)
		{
			this->asteroidDirectionRadian = asin(abs(posX - playerX) / distance);
		}
		else
		{
			this->asteroidDirectionRadian = 2 * PI - asin(abs(posX - playerX) / distance);
		}
	}
	else
	{
		if (posX > playerX)
		{
			this->asteroidDirectionRadian = PI - asin(abs(posX - playerX) / distance);
		}
		else
		{
			this->asteroidDirectionRadian = PI + asin(abs(posX - playerX) / distance);
		}
	}
	this->asteroidSpeed = rand() % ASTEROID_SPEED + 250;
	this->velocityX = 0.0f;
	this->velocityY = 0.0f;
	this->vertices = rand() % 10 + 5;
	this->rotateSpeed = rand() % 6 - 3;
	this->asteroidRotateRadian = 0.0f;
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

void Asteroid::rotate()
{
	float angle = (double)asteroidRotateRadian * 180 / PI;
	angle += rotateSpeed;
	if ((int)angle % 360 == 0)
	{
		angle = 0;
	}
	asteroidRotateRadian = angle * PI / 180;
}

void Asteroid::move()
{
	posX += velocityX;
	posY += velocityY;
}

void Asteroid::getHit()
{
	hp -= PLAYER_DAMGE;
}