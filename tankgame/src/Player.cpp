#include "Player.h"

Player::Player(int playerPosX, int playerPosY)
{
	this->playerPosX = playerPosX;
	this->playerPosY = playerPosY;
}

void Player::move(Direction dir)
{
	if(dir == N)
	{
		playerPosY++;
	}
	else if(dir == S)
	{
		playerPosY--;
	}
	else if(dir == E)
	{
		playerPosX++;
	}
	else if(dir == W)
	{
		playerPosX--;
	}
}

void Player::shoot()
{
	
}