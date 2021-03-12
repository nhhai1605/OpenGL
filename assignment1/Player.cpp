#include "Player.h"

Player::Player(float playerX, float playerY)
{
	this->playerX = playerX;
	this->playerY = playerY;
}

void Player::setPlayerPos(float x, float y)
{
	this->playerX = x;
	this->playerY = y;
}

void Player::move()
{
	this->playerY += 0.1;
}