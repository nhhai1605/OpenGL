#include "Particle.h"

Particle::Particle(Player* player, float size, float color[3])
{
	this->size = size;
	this->initSize = size;
	int offset = rand() % 60 - 30;
	this->particleDirectionRadian = player->playerDirectionRadian - PI + offset * PI /180;
	this->posX = player->posX - 10 * sin(particleDirectionRadian);
	this->posY = player->posY +  10 * cos(particleDirectionRadian);
	this->velocityX = 0.0f;
	this->velocityY = 0.0f;
	this->color[0] = color[0] + (rand() % 5 - 3) * 0.1f;
	this->color[1] = color[1] + (rand() % 5 - 3) * 0.1f;
	this->color[2] = color[2] + (rand() % 5 - 3) * 0.1f;
}

Particle::Particle(float posX, float posY, float radius, float radian, float color[3], float size)
{
	this->size = size;
	this->initSize = size;
	this->notExplosion = false;
	this->posX = posX;
	this->posY = posY;
	this->spawnX = posX;
	this->spawnY = posY;
	this->velocityX = 0.0f;
	this->velocityY = 0.0f;
	this->particleDirectionRadian = radian;
	this->radius = rand() % (int)radius;
	this->color[0] = color[0] + (rand() % 3 - 1) * 0.1f;
	this->color[1] = color[1] + (rand() % 3 - 1) * 0.1f;
	this->color[2] = color[2] + (rand() % 3 - 1) * 0.1f;
}

Particle::~Particle()
{
	std::cout << "deleted" << std::endl;
}


void Particle::move()
{
	if (notExplosion)
	{
		posX += velocityX;
		posY += velocityY;
	}
	else 
	{
		if (radius <= 0)
		{
			velocityX = 0;
			velocityY = 0;
		}
		else
		{
			posX += velocityX;
			posY += velocityY;
		}
		radius -= sqrt(velocityX * velocityX + velocityY * velocityY);
	}
	colorChange();
}

void Particle::colorChange()
{
	this->color[1] += size / initSize / 50.0f;
	this->color[2] += size / initSize / 500.0f;
}