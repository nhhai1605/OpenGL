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
	this->playerSize = INIT_PLAYER_SIZE;
}

void Player::move()
{
	posX += velocityX;
	posY += velocityY;
}

void Player::setPlayerDirection(float x, float y)
{
	float currentX = x - posX;
	float currentY = y - posY;
	float hypotenuse = sqrt(currentX * currentX + currentY * currentY);
	float radian = -(float)asin(currentX / hypotenuse);
	if(y >= posY)
		this->playerDirectionRadian = radian;
	else
		this->playerDirectionRadian = PI - radian;
}

void Player::shoot()
{

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