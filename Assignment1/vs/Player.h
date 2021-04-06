#ifndef PLAYER
#define PLAYER
#include "Utility.h"
class Player
{
public:
	Player(float posX, float posY, float initPlayerSize);
	~Player();
	float posX, posY, playerDirectionRadian = 0.0f, thrustSpeed, playerSize, initPlayerSize;
	float velocityX = 0.0f, velocityY = 0.0f;
	int life = LIFE, score = 0, asteroidsDestroyed = 0;
	float spawningX, spawningY;
	void move();
	void setPlayerPos(float x, float y);
	void rotateLeft();
	void rotateRight();
	void die();
	void respawn();
private:
};

#endif // PLAYER
 