#ifndef PLAYER
#define PLAYER
#include "Direction.h"
class Player
{
public:
	int playerPosX, playerPosY;
	Direction gunDir = N;
	Direction lastGunDir = NONE;
	Player(int playerPosX, int playerPosY);
	~Player();
	void move(Direction dir);
	void shoot();
private:
	
};

#endif // PLAYER
