#ifndef BULLET
#define BULLET
#include <math.h>
#include <iostream>
class Bullet
{
public:
	Bullet(float bulletX, float bulletY, float bulletDirectionRadian);
	~Bullet();
	float bulletX, bulletY, bulletDirectionRadian, bulletSpeed = 10.0f;
	void setBulletPos(float x, float y);
	//void setBulletDirection(float x, float y);
	//void setBulletDirection(float angle);
	void move();
private:
};

#endif // BULLET
