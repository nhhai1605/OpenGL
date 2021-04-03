#include "Bullet.h"

Bullet::Bullet(float posX, float posY, float bulletDirectionRadian, float size)
{
	this->posX = posX;
	this->posY = posY;
	velocityX = 0.0f;
	velocityY = 0.0f;
	this->size = size;
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

void Bullet::move()
{
	posX += velocityX;
	posY += velocityY;
}

