#pragma once
#include "Utility.h"
#include "Player.h"
class Particle
{
public:
	vector3 position, velocity, spawn;
	float horRadian = 0.0f, verRadian = 0.0f, notExplosion = true;
	float size, initSize, color[4], expRadius = 0.0f;
	Particle(Player* player, float size, float color[4])
	{
		this->size = size;
		this->initSize = size;
		int offset = rand() % 180 - 90;
		this->horRadian = -player->horRadian + offset * PI / 180;
		this->verRadian = -player->verRadian + offset * PI / 180;
		this->position = player->back;
		vector3 randPos = vector3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10);
		this->position = this->position + randPos;
		this->velocity = vector3(0.0, 0.0, 0.0);
		this->color[0] = color[0] + (rand() % 5 - 1) * 0.1f;
		this->color[1] = color[1] + (rand() % 5 - 1) * 0.1f;
		this->color[2] = color[2] + (rand() % 5 - 1) * 0.1f;
		this->color[3] = color[3];
	}

	Particle(vector3 postion, float radius, float horRadian, float verRadian, float color[4], float size)
	{
		this->size = size;
		this->initSize = size;
		this->notExplosion = false;
		this->position = postion;
		this->spawn = postion;
		this->velocity = vector3(0.0, 0.0, 0.0);
		this->horRadian = horRadian;
		this->verRadian = verRadian;
		this->expRadius = (float)(rand() % (int)radius);
		this->color[0] = color[0] + (rand() % 5 - 1) * 0.1f;
		this->color[1] = color[1] + (rand() % 5 - 1) * 0.1f;
		this->color[2] = color[2] + (rand() % 5 - 1) * 0.1f;
		this->color[3] = color[3];
	}

	~Particle()
	{
	}


	void update()
	{
		if (notExplosion)
		{
			this->position = this->position + this->velocity;
		}
		else
		{
			if (expRadius <= 0)
			{
				this->velocity = vector3(0.1f, 0.1f, 0.1f);
			}
			else
			{
				this->position = this->position + this->velocity;
			}
			expRadius -= magnitude(this->velocity);
		}
		colorChange();
	}

	void colorChange()
	{
		this->color[1] += size / initSize / 50.0f;
		this->color[2] += size / initSize / 500.0f;
	}
private:
};

