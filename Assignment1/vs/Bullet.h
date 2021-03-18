#ifndef BULLET
#define BULLET
#include "Utility.h"
class Bullet
{
public:
	Bullet(float posX, float posY, float bulletDirectionRadian);
	~Bullet();
	float posX, posY, bulletDirectionRadian;
	void setBulletPos(float x, float y);

private:
};

#endif // BULLET
