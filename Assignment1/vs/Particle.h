#ifndef PARTICLE
#define PARTICLE
#include "Utility.h"
#include "Player.h"
class Particle
{
public:
	Particle(Player * player, float size, float color[3]);
	Particle(float posX, float posY, float radius, float radian, float color[3], float size);
	~Particle();
	float posX, posY, particleDirectionRadian = 0.0f, notExplosion = true;;
	float size, initSize, velocityX, velocityY, color[3], radius = 0.0f, spawnX = 0.0f, spawnY = 0.0f;
	void move();
	void colorChange();
private:
};

#endif // PARTICLE
