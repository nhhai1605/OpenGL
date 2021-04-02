#ifndef PLAYER
#define PLAYER
#include "Utility.h"
class Player
{
public:
	Player(float posX, float posY);
	~Player();
	float posX, posY, playerDirectionRadian = 0.0f, playerSize = PLAYER_SIZE, thrustSpeed = playerSize / 2;
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
 