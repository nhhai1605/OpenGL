#ifndef EXPLOSION
#define EXPLOSION
#include "Utility.h"
#include "Particle.h"
class Explosion
{
public:
	Explosion(float posX, float posY, float radius, float color[3], int particleNum, float particleSize, float scale);
	~Explosion();
	float posX, posY, radius, particleSize;
	float decayTime = EXPLOSION_DECAY_TIME;
	float sizeReduceRate;
	std::vector<Particle*> particles;
private:
};

#endif // EXPLOSION
