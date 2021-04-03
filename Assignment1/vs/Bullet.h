#ifndef BULLET
#define BULLET
#include "Utility.h"
class Bullet
{
public:
	Bullet(float posX, float posY, float bulletDirectionRadian, float size);
	~Bullet();
	float posX, posY, bulletDirectionRadian, velocityX, velocityY, size;
	void setBulletPos(float x, float y);
	void move();
private:
};

#endif // BULLET
