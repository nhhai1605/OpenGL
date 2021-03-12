#ifndef PLAYER
#define PLAYER
class Player
{
public:
	Player(float playerX, float playerY);
	~Player();
	float playerX, playerY;
	void setPlayerPos(float x, float y);
	void move();
private:
};

#endif // PLAYER
