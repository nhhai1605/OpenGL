#include "Explosion.h"

Explosion::Explosion(float posX, float posY, float radius, float color[3], int particleNum, float particleSize, float scale)
{
	this->posX = posX;
	this->posY = posY;
	this->radius = radius;
	this->particleSize = particleSize;
	float angle = 360.0f / particleNum;
	float colorArr[] = { color[0], color[1], color[2] };
	for (int i = 0; i < particleNum; i++)
	{
		float radian = (angle + rand() % 20 - 10) * PI / 180;
		Particle* particle = new Particle(posX, posY, radius, radian * i, colorArr, particleSize);
		particle->velocityX = scale * EXPLOSION_SPEED / 60 * -sin(particle->particleDirectionRadian);
		particle->velocityY = scale * EXPLOSION_SPEED / 60 * cos(particle->particleDirectionRadian);
		particles.push_back(particle);
	}
	sizeReduceRate = particleSize / EXPLOSION_DECAY_TIME;
}


Explosion::~Explosion()
{

}
