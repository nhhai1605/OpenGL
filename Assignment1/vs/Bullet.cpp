#include "Bullet.h"

Bullet::Bullet(float bulletX, float bulletY, float bulletDirectionRadian)
{
	this->bulletX = bulletX;
	this->bulletY = bulletY;
	this->bulletDirectionRadian = bulletDirectionRadian;
}
Bullet::~Bullet()
{

}

void Bullet::setBulletPos(float x, float y)
{
	this->bulletX = x;
	this->bulletY = y;
}

//void Bullet::setBulletDirection(float x, float y)
//{
//	float d = sqrt(x * x + y * y);
//	float angle = -(float)asin(x / d) * 180.0 / 3.1415;
//	this->bulletDirection = angle;
//}
//
//void Bullet::setBulletDirection(float angle)
//{
//	this->bulletDirection = angle;
//}

void Bullet::move()
{
	bulletY += bulletSpeed;
}
