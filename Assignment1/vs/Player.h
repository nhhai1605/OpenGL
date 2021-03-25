#ifndef PLAYER
#define PLAYER
#include "Utility.h"
class Player
{
public:
	Player(float posX, float posY);
	~Player();
	float posX, posY, playerDirectionRadian = 0.0f, playerSize = INIT_PLAYER_SIZE, thrustSpeed = playerSize / 2;
	//it shouldnt be called velocity but it includes velocity, acceleration, momentum so i just let it be velociy xD
	float velocityX = 0.0f, velocityY = 0.0f;
	int live = 3, score = 0, asteroidsDestroyed = 0;
	void move();
	void setPlayerPos(float x, float y);
	void rotateLeft(float dt);
	void rotateRight(float dt);
	void die();
private:
};

#endif // PLAYER
 