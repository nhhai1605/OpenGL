#include "Particle.h"

Particle::Particle(float posX, float posY, Player* player)
{
	this->posX = posX;
	this->posY = posY;
	particleDirectionRadian = player->playerDirectionRadian;
}


Particle::~Particle()
{

}


void Particle::move()
{

}