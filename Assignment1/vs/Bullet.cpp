#include "Bullet.h"

Bullet::Bullet(float posX, float posY, float bulletDirectionRadian)
{
	this->posX = posX;
	this->posY = posY;
	this->bulletDirectionRadian = bulletDirectionRadian;
}
Bullet::~Bullet()
{

}

void Bullet::setBulletPos(float x, float y)
{
	this->posX = x;
	this->posY = y;
}


