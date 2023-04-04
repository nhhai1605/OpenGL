#pragma once
#include "Utility.h"
class Player
{
public:
	float horRadian = 0.0f, verRadian = 0.0f, thrustSpeed, playerSize, initPlayerSize;
	vector3 position, velocity, spawning, front, back;
	int life = LIFE, score = 0, asteroidsDestroyed = 0;
	Player(float x, float y, float z, float thrustSpeed = THRUST_SPEED, float initPlayerSize = PLAYER_SIZE)
	{
		this->position = vector3(x, y, z);
		this->velocity = vector3(0.0f, 0.0f, 0.0f);
		this->spawning = vector3(x, y, z);
		this->initPlayerSize = initPlayerSize;
		this->playerSize = initPlayerSize;
		this->thrustSpeed = thrustSpeed;
	}
	Player(vector3 pos, float initPlayerSize = PLAYER_SIZE)
	{
		this->position = pos;
		this->velocity = vector3(0.0f, 0.0f, 0.0f);
		this->spawning = pos;
		this->initPlayerSize = initPlayerSize;
		this->playerSize = initPlayerSize;
		thrustSpeed = initPlayerSize / 2;
	}
	~Player() 
	{

	}

	void setPlayerPos(float x, float y, float z)
	{
		this->position = vector3(x, y, z);
	}

	void die()
	{
		position = spawning;
		velocity = vector3(0.0f, 0.0f, 0.0f);
		horRadian = 0.0f;
		verRadian = 0.0f,
		playerSize = initPlayerSize;
		life--;
	}

	void respawn()
	{
		life = LIFE;
		position = spawning;
		velocity = vector3(0.0f, 0.0f, 0.0f);
		horRadian = 0.0f;
		verRadian = 0.0f,
		playerSize = initPlayerSize;
		asteroidsDestroyed = 0;
		score = 0;
	}


	void rotateLeft()
	{
		float angle = (float)((double)horRadian * 180.0f / PI);
		angle -= ROTATION_SPEED;
		if (angle >= 180)
		{
			angle -= 360;
		}
		horRadian = angle * PI / 180.0f;
	}

	void rotateRight()
	{
		float angle = (float)((double)horRadian * 180.0f / PI);
		angle += ROTATION_SPEED;
		if (angle <= -180)
		{
			angle += 360.0f;
		}
		horRadian = angle * PI / 180.0f;
	}

	void rotateUp()
	{
		float angle = (float)((double)verRadian * 180.0f / PI);
		angle -= ROTATION_SPEED;
		if (angle <= -180)
		{
			angle += 360.0f;
		}
		verRadian = angle * PI / 180.0f;
	}

	void rotateDown()
	{
		float angle = (float)((double)verRadian * 180.0f / PI);
		angle += ROTATION_SPEED;
		if (angle >= 180)
		{
			angle -= 360;
		}
		verRadian = angle * PI / 180.0f;
	}
	void update()
	{
		position = position + velocity;
		vector3 front;
		front.y = -playerSize * sin(verRadian) * 1.5f;
		front.x = -playerSize * sin(horRadian) * 1.5f * cos(verRadian);
		front.z = playerSize * cos(horRadian) * 1.5f * cos(verRadian);
		this->front = this->position + front;
		this->back = this->position - front;
	}
};

 