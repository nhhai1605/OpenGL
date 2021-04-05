#include "Player.h"
Player::Player(float posX, float posY)
{
	this->posX = posX;
	this->posY = posY;
	this->spawningX = posX;
	this->spawningY = posY;
}

Player::~Player()
{

}

void Player::setPlayerPos(float x, float y)
{
	this->posX = x;
	this->posY = y;
}

void Player::die()
{
	posX = spawningX;
	posY= spawningY;
	velocityX = 0.0;
	velocityY = 0.0;
	playerDirectionRadian = 0.0f;
	playerSize = PLAYER_SIZE;
	life--;
}

void Player::move()
{
	posX += velocityX;
	posY += velocityY;
}

void Player::respawn()
{
	life = LIFE;
	posX = spawningX;
	posY = spawningY;
	velocityX = 0.0;
	velocityY = 0.0;
	playerDirectionRadian = 0.0f;
	playerSize = PLAYER_SIZE;
	asteroidsDestroyed = 0;
	score = 0;
}


void Player::rotateLeft()
{
	float angle = (float)((double)playerDirectionRadian * 180.0f / PI);
	angle += ROTATION_SPEED;
	if (angle >= 360)
	{
		angle -= 360;
	}
	playerDirectionRadian = angle * PI / 180.0f;
}

void Player::rotateRight()
{
	float angle = (float)((double)playerDirectionRadian * 180.0f / PI);
	angle -= ROTATION_SPEED;
	if (angle <= 360)
	{
		angle += 360.0f;
	}
	playerDirectionRadian = angle * PI / 180.0f;
}