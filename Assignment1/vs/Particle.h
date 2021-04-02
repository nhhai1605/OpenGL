#ifndef PARTICLE
#define PARTICLE
#include "Utility.h"
#include "Player.h"
class Particle
{
public:
	Particle(float posX, float posY, Player * player);
	~Particle();
	float posX, posY, particleDirectionRadian = 0.0f;
	void move();
private:
};

#endif // PARTICLE
