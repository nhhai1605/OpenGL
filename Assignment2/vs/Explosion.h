#pragma once
#include "Utility.h"
#include "Particle.h"
class Explosion
{
public:
	vector3 position;
	float radius, particleSize;
	float decayTime = EXPLOSION_DECAY_TIME;
	float sizeReduceRate;
	float color[4];
	std::vector<Particle*> particles;
	Player* player;
	Explosion(vector3 position, Player* player, float radius, float color[4], int particleNum, float particleSize)
	{
		this->player = player;
		this->position = position;
		float distance = magnitude(player->position - position);
		this->radius = radius * distance / NORM_DISTANCE;
		this->particleSize = particleSize;
		float angle = 360.0f / particleNum;
		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
		this->color[3] = color[3];
		for (int i = 0; i < particleNum; i++)
		{
			float horRadian = (angle + rand() % 20 - 10) * PI / 180;
			float verRadian = (angle + rand() % 20 - 10) * PI / 180;
			Particle* particle = new Particle(position, radius, horRadian * i, verRadian * i, color, particleSize);
			particle->velocity.x = EXPLOSION_SPEED / 60 * -sin(particle->horRadian) * cos(particle->verRadian);
			particle->velocity.y = EXPLOSION_SPEED / 60 * sin(particle->verRadian);
			particle->velocity.z = EXPLOSION_SPEED / 60 * cos(particle->horRadian) * cos(particle->verRadian);
			particles.push_back(particle);
		}
		sizeReduceRate = particleSize / EXPLOSION_DECAY_TIME;
	}
	~Explosion()
	{

	}
private:
};

