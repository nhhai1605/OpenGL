#ifndef PLAYER
#define PLAYER
#include <math.h>
#include <iostream>

class Player
{
public:
	Player(float posX, float posY);
	~Player();
	float posX, posY, playerDirectionRadian, playerSize;
	float velocityX = 0.0f, velocityY = 0.0f;
	void move(float dt);
	void setPlayerPos(float x, float y);
	void setPlayerDirection(float x, float y);
	void shoot();
	void rotateLeft(float dt);
	void rotateRight(float dt);
private:
};

#define INIT_PLAYER_SIZE 10.0f
#define MAX_SPEED 30.0f
#define ROTATION_SPEED 5.0f
#define PI 3.14159265f
#define THRUST_SPEED 5.0f
#define FRICTION 0.8f

#endif // PLAYER
 