#include "Player.h"
Player::Player(float posX, float posY)
{
	this->posX = posX;
	this->posY = posY;
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
	posX = 0.0;
	posY= 0.0;
	live--;
}

void Player::move()
{
	posX += velocityX;
	posY += velocityY;
}

void Player::rotateLeft(float dt)
{
	float angle = (double)playerDirectionRadian * 180 / PI;
	angle += ROTATION_SPEED;
	if ((int)angle % 360 == 0)
	{
		angle = 0;
	}
	playerDirectionRadian = angle * PI / 180;
}

void Player::rotateRight(float dt)
{
	float angle = (double)playerDirectionRadian * 180 / PI;
	angle -= ROTATION_SPEED;
	if ((int)angle % 360 == 0)
	{
		angle = 0;
	}
	playerDirectionRadian = angle * PI / 180;
}