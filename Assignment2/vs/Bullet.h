#pragma once

#include "Utility.h"
class Bullet
{
public:
	float horRadian, verRadian, size, speed;
	vector3 position, velocity;
	Bullet(Player* player, float size = BULLET_SIZE, float speed = BULLET_SPEED)
	{
		this->horRadian = player->horRadian;
		this->verRadian = player->verRadian;
		this->position = player->front;
		this->size = size;
		this->speed = speed;
		velocity = vector3(0.0f, 0.0f, 0.0f);
	}
	~Bullet()
	{

	}

	void setBulletPos(vector3 position)
	{
		this->position = position;
	}

	void move()
	{
		this->position = this->position + this->velocity;
	}
private:
};


