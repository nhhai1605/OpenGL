#pragma once
#include "Utility.h"
class Asteroid
{
public:
	float asteroidSpeed, mass, radius, hp;
	float rotateSpeed, asteroidRotateRadian, asteroidHorRadian;
	vector3 position, velocity, target;
	std::vector <float> offsetsHor;
	std::vector <float> offsetsVer;
	AsteroidState state;
	int vertices;
	Asteroid(vector3 position, vector3 target, float radius, int asteroidSpeed = ASTEROID_SPEED, AsteroidState state = AsteroidState::SPLITTABLE)
	{
		this->position = position;
		this->target = target;
		this->radius = radius;
		this->mass = radius;
		this->state = state;
		this->hp = ceil(radius / 20) * 10;
		this->asteroidSpeed = (float)(rand() % asteroidSpeed + asteroidSpeed);
		this->velocity = vector3(0.0f, 0.0f, 0.0f);
		this->vertices = rand() % 5 + 5;
		this->rotateSpeed = (float)(rand() % 5 - 2);
		this->asteroidRotateRadian = 0.0f;
		float distance = sqrt((this->position.x - this->target.x) * (this->position.x - this->target.x) + (this->position.z - this->target.z) * (this->position.z - this->target.z));

		if (this->position.z < this->target.z)
		{
			if (this->position.x > this->target.x)
			{
				this->asteroidHorRadian = asin(abs(this->position.x - this->target.x) / distance);
			}
			else
			{
				this->asteroidHorRadian = 2 * PI - asin(abs(this->position.x - this->target.x) / distance);
			}
		}
		else
		{
			if (this->position.x > this->target.x)
			{
				this->asteroidHorRadian = PI - asin(abs(this->position.x - this->target.x) / distance);
			}
			else
			{
				this->asteroidHorRadian = PI + asin(abs(this->position.x - this->target.x) / distance);
			}
		}
		for (int i = 0; i < vertices; i++)
		{
			offsetsHor.push_back(angleToRadian(rand() % 360));
			offsetsVer.push_back(angleToRadian(rand() % 360));
		}
	}

	~Asteroid()
	{
	}

	void setAsteroidPos(float x, float y, float z)
	{
		this->position = vector3(x, y, z);
	}

	void rotate()
	{
		float angle = (float)(asteroidRotateRadian * 180 / PI);
		angle += rotateSpeed;
		if (angle >= 180)
		{
			angle -= 360;
		}
		if (angle <= -180)
		{
			angle += 360.0f;
		}
		asteroidRotateRadian = angle * PI / 180;
	}

	void update()
	{
		this->position = this->position + this->velocity;
		rotate();
	}

	void getHit()
	{
		hp -= PLAYER_DAMGE;
	}
private:
};


