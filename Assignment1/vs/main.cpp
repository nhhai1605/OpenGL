#include "Player.h"
#include "Bullet.h"
#include "Utility.h"
#include "Asteroid.h"
#include "Particle.h"
#include "Explosion.h"

//true for seeing the objects' hitbox
#define SHOW_HITBOX true

//false to activate the "touch wall then die" mode
#define GO_THROUGH_WALL false

//true to spawning in the middle, false to set it manually, position will base on the arena width and height, not screen width and height
#define SPAWNING_IN_MIDDLE true 
#define SPAWNING_POS 200.0f //if set false, you can change the spawning position here, i let both X and Y the same 

#define SHOW_FPS true

// false for the simple pulse mode and false for spiral mode
#define BH_SPIRAL false

//true to let the arena fit the screen, fasle to set it manually
#define ARENA_FIT_SCREEN true
float arenaWidth = 800, arenaHeight = 400; //if false, you can set the arena width and height here
//Arena color
float arena_color[] = { 0.0f, 0.0f, 0.0f };

//Player color
float player_outline_color[] = { 1.0, 0.0, 0.0 }; //player outline color
float player_fill_color[] = {1.0, 1.0, 1.0}; //player fill color

//Boost particle
int particleNum = 200; //max number of particles 
float decayTime = 2000.0f; // decay time of each particle (ms)
float sizeReduceRate = INIT_PARTICLE_SIZE / decayTime; //the reduce rate of particle size in ms
float particle_color[] = { 1.0f, 0.2f, 0.02f}; //particle init color (before any change when decaying)
float particleSpeed = 200.0f; //particle speed when going out from the back of the ship
float timeBetweenParticles = 10.0f; //time delaying before each particle
float particle_dt = 0.0f; 

//Explosion
int explosionParticles = 200; 
float explosion_color[] = { 1.0f, 0.0f, 0.0f };
//Bullet color
float bullet_color[] = { 1.0, 0.0, 0.0 };

//Wall Warning color
float warning_color[] = { 1.0, 0.0, 0.0 };

//Black hole
float blackHoleX = 0.0f, blackHoleY = 0.0f; //BH position, which are initialized randomly in the main function
float bh_color[] = { 0.5f, 0.2f, 0.5f };
float bh_dt = 0.0f;
float blackHolePullingRate = 2.5f; //this variable is used to make the object smaller when being pulled by the BH
float blackHoleFrequency = 5000.0f; // this is the frequency of the BH, e.g. 5s off then 5s on
bool blackHoleActivated = false; //this is to keep track of the BH state
//These 2 variables are used for the pulse mode (the circle going inward)
float pulseSpeed = 2.5f;
float pulseVel = 0.0f;

//change number of init asteroids here
int currRound = 1; //current round
int asteroids_number = INIT_ASTEROIDS_NUMBER * currRound; //number of asteroids in each rounds
float asteroid_spawn_rate = 2000.0f; //time delay between each spawn (ms)

Player* player;
std::vector <Bullet*> bullets;
std::vector <Asteroid*> asteroids;
KeyState keyArray[127], mousePressed;
std::vector <WallWarning> warnings;
std::vector <Particle*> particles;
std::vector <Explosion*> explosions;

//Global variables
float screenWidth, screenHeight; //screen width and height of user's screen
float last_t = 0.0f, dt = 0.0f; //this is delta time, which are used in many cases
float currentFPS = 0.0f, lastFPS = 0.0f; //these are used for displaying fps
float shoot_delay = 0.0f, fps_update_delay = 0.0f, asteroid_spawn_delay = 0.0f; //these variables are used for delaying between each bullets, each asteroids,..
//These boolean variables are used to keep tracked of player state or game state
bool shootable = true;
bool isThrusting = false;
bool FPSupdatable = true;
bool gameStarted = false;
bool playerDied = false;

float time_since_game_start = 0.0f; // this is used to calculate time in game

//these are used to calculate and displaying the blinking and invulnerable time after you die
float after_die_time = 0.0f;
bool invulnerable_state = false;
int blinking_num = INVULNERABLE_TIME / BLINKING_DUR;	
float blinking_time = BLINKING_DUR;

//this is a simple function used to calculate distance between 2 points
float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

//This function is called when player have no life left, and choose to restart again.
void restartGame()
{
	//set every thing back at the beginning state and also make a black hole at other random position
	playerDied = false;
	player->respawn();
	asteroids.clear();
	particles.clear();
	explosions.clear();
	currRound = 1;
	int asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
	blackHoleX = (rand() % (int)(arenaWidth - 2 * BLACK_HOLE_RADIUS)) + (screenWidth - arenaWidth) / 2 + BLACK_HOLE_RADIUS;
	blackHoleY = (rand() % (int)(arenaHeight - 2 * BLACK_HOLE_RADIUS)) + (screenHeight - arenaHeight) / 2 + BLACK_HOLE_RADIUS;
}

//this function is used to draw a black hole
void drawBlackHole(float posX, float posY)
{
	glColor3f(bh_color[0], bh_color[1], bh_color[2]);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (float i = -BLACK_HOLE_RADIUS; i <= BLACK_HOLE_RADIUS; i += 0.1f)
	{
		float circleX = i;
		float circleY = sqrt(BLACK_HOLE_RADIUS * BLACK_HOLE_RADIUS - circleX * circleX);
		glVertex3f(circleX + posX, posY + circleY, -0.5);
		glVertex3f(circleX + posX, posY - circleY, -0.5);
	}
	glEnd();
	if (blackHoleActivated)
	{
		if (BH_SPIRAL)
		{
			int space = 5;
			glBegin(GL_POINTS);
			glColor3f(bh_color[0], bh_color[1], bh_color[2]);
			for (int i = 0; i < 300; i++)
			{
				float angle = 0.1f * i;
				float r = (float)(glutGet(GLUT_ELAPSED_TIME) / 40.0f);
				float x = (space * angle) * cos(angle - r);
				float y = (space * angle) * sin(angle - r);
				glVertex3f(x + posX, posY + y, -0.5);
			}
			glEnd();
		}
		else
		{
			pulseVel += pulseSpeed;
			glBegin(GL_POINTS);
			float newR = BLACK_HOLE_RADIUS - pulseVel;
			for (float j = 0.0f; BLACK_HOLE_RADIUS - pulseVel + j <= BLACK_HOLE_RADIUS; j += pulseSpeed * 10)
			{
				newR = BLACK_HOLE_RADIUS - pulseVel + j;
				for (float i = -newR; i <= newR; i += 0.1f)
				{
					float circleX = i;
					float circleY = sqrt(newR * newR - circleX * circleX);
					glVertex3f(circleX + posX, posY + circleY, -0.5);
					glVertex3f(circleX + posX, posY - circleY, -0.5);
				}
			}
			glEnd();
		}
	}

}

///this function is used to draw an arena, including changing color of the wall when player is near
void drawArena(float arenaWidth, float arenaHeight)
{
	glLineWidth(5.0f); 
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	boolean w = false, s = false, n = false, e = false;
	for (WallWarning warning : warnings)
	{
		if (warning == WallWarning::W && gameStarted && !playerDied)
		{
			w = true;
		}
		if (warning == WallWarning::S && gameStarted && !playerDied)
		{
			s = true;
		}
		if (warning == WallWarning::E && gameStarted && !playerDied)
		{
			e = true;
		}
		if (warning == WallWarning::N && gameStarted && !playerDied)
		{
			n = true;
		}
	}
	if (w)
	{
		glColor3f(warning_color[0], warning_color[1], warning_color[2]);
	}
	float leftCornerX = (float)((screenWidth - arenaWidth) / 2);
	float leftCornerY = (float)((screenHeight - arenaHeight) / 2);
	glVertex3f(leftCornerX, leftCornerY, 0.0);
	glVertex3f(leftCornerX, leftCornerY + arenaHeight, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	if (n)
	{
		glColor3f(warning_color[0], warning_color[1], warning_color[2]);
	}
	glVertex3f(leftCornerX, leftCornerY + arenaHeight, 0.0);
	glVertex3f(leftCornerX + arenaWidth, leftCornerY + arenaHeight, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	if (e)
	{
		glColor3f(warning_color[0], warning_color[1], warning_color[2]);
	}
	glVertex3f(leftCornerX + arenaWidth, leftCornerY + arenaHeight, 0.0);
	glVertex3f(leftCornerX + arenaWidth, leftCornerY, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	if (s)
	{
		glColor3f(warning_color[0], warning_color[1], warning_color[2]);
	}
	glVertex3f(leftCornerX + arenaWidth, leftCornerY, 0.0);
	glVertex3f(leftCornerX, leftCornerY, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glEnd();
}

///this function is used to draw player's information, like life left, score, number of destroyed asteroids and time has passed in the game
void drawPlayerInfo(Player * player)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, 0 , screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);

	std::string live = "Life: " + std::to_string(player->life);
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (live.size() - 3), (screenHeight - arenaHeight) / 2 + arenaHeight - 25);
	for (std::string::iterator i = live.begin(); i != live.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	std::string score = "Score: " + std::to_string(player->score);\
	glRasterPos2f((screenWidth - arenaWidth) / 2 + 10, (screenHeight - arenaHeight) / 2 + arenaHeight - 25);
	for (std::string::iterator i = score.begin(); i != score.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	std::string asteroidsDestroyed = "Destroyed: " + std::to_string(player->asteroidsDestroyed);
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (asteroidsDestroyed.size() + 1), (screenHeight - arenaHeight) / 2 + arenaHeight - 25);
	for (std::string::iterator i = asteroidsDestroyed.begin(); i != asteroidsDestroyed.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	int t = (int)time_since_game_start;
	int minutes = (int)floor(t / 60000);
	int seconds = (int)floor((t - minutes * 60000) / 1000);
	std::string time = "Time: " + std::to_string(minutes) + ":" + std::to_string(seconds);
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth + 10 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (time.size() + 2), (screenHeight - arenaHeight) / 2 + 10);
	for (std::string::iterator i = time.begin(); i != time.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	std::string round = "Round: " + std::to_string(currRound);
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (round.size() - 3), (screenHeight - arenaHeight) / 2 + 10);
	for (std::string::iterator i = round.begin(); i != round.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//this function is used to draw the FPS string at the bottom left of the screen
void drawFPS(std::string fps)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f((screenWidth - arenaWidth) / 2 + 10, (screenHeight - arenaHeight) / 2 + 10);
	fps = "FPS: " + fps;
	for (std::string::iterator i = fps.begin(); i != fps.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

///this function is used to draw player's ship, including thrusting particles
void drawPlayer(Player * player)
{
	float x = player->posX, y = player->posY;
	float size = player->playerSize;
	float radian = player->playerDirectionRadian;
	//x' = x * cos(theta) - y * sin(theta)
	//y' = x * sin(theta) + y * cos(theta)
	//with x and y is 0 and theta is playerDirectionRadian
	//when player move, we count new playerX and playerY is 0, so x and y is 0
	float x1, x2, x3, y1, y2, y3;
	x1 = size * cos(radian) + size * sin(radian);
	y1 = size * sin(radian) - size * cos(radian);
	x2 = -size * 1.5f * sin(radian);
	y2 = size * 1.5f * cos(radian);
	x3 = -size * cos(radian) + size * sin(radian);
	y3 = -size * sin(radian) - size * cos(radian);
	
	if (invulnerable_state)
	{
		//blinking here is used to make player blink when they are in the invulnerable state (after respawning)
		if (blinking_num % 2 == 0)
		{
			glColor3f(player_fill_color[0], player_fill_color[1], player_fill_color[2]);
		}
		else
		{
			glColor3f(arena_color[0], arena_color[1], arena_color[2]);
		}
		//draw the ship
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glEnd();

		if (blinking_num % 2 == 0)
		{
			glColor3f(player_outline_color[0], player_outline_color[1], player_outline_color[2]);
		}
		else
		{
			glColor3f(arena_color[0], arena_color[1], arena_color[2]);
		}
		//draw the ship's outline
		glLineWidth(player->playerSize / 6);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glVertex3f(x, y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glEnd();

		//this is used to managed the blinking effect
		if (blinking_num > 0)
		{
			blinking_time -= dt;
			if (blinking_time <= 0)
			{
				blinking_time = BLINKING_DUR;
				blinking_num--;
			}
		}
		else
		{
			blinking_num = INVULNERABLE_TIME / BLINKING_DUR;
		}
	}
	else
	{	
		//this is just the same, but when the player is not in the invulnerable state
		glColor3f(player_fill_color[0], player_fill_color[1], player_fill_color[2]);
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glEnd();
		glColor3f(player_outline_color[0], player_outline_color[1], player_outline_color[2]);
		glLineWidth(player->playerSize / 6);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glVertex3f(x, y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glEnd();
	}
	//this is used to draw paticles
	for (auto& particle : particles)
	{
		glPointSize(particle->size);
		glColor3f(particle->color[0], particle->color[1], particle->color[2]);
		glBegin(GL_POINTS);
		glVertex3f(particle->posX, particle->posY, 0.0);
		glEnd();
	}

	//this is used to draw the circle hitbox outside of the ship
	if (SHOW_HITBOX == true)
	{
		float r = player->playerSize;
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (float i = -r; i <= r; i += 0.01f)
		{
			float circleX = i;
			float circleY = sqrt(r * r - circleX * circleX);
			glVertex3f(circleX + player->posX, player->posY + circleY, 1.0);
			glVertex3f(circleX + player->posX, player->posY - circleY, 1.0);
		}
		glEnd();
	}
}

///this function is used to draw the bullet
void drawBullet(Bullet * bullet)
{
	glPointSize(bullet->size);
	glColor3f(bullet_color[0], bullet_color[1], bullet_color[2]);
	glBegin(GL_POINTS);
	glVertex3f(bullet->posX, bullet->posY, 0.0);
	glEnd();
}

//this function is used to draw the asteroid
void drawAsteroid(Asteroid* asteroid)
{
	glColor3f(0.5, 0.5, 0.5);
	float angle = 360.0f / asteroid->vertices; //the vertices are randomly chosen when the asteroid is made
	float asteroidRadian = asteroid->asteroidRotateRadian;
	float radius = asteroid->radius;
	glBegin(GL_POLYGON);
	float radian = angle * PI / 180;
	for (int i = 0; i < asteroid->vertices; i++)
	{
		float x = radius * cos(radian * i + asteroidRadian) + radius * sin(radian * i + asteroidRadian);
		float y = radius * sin(radian * i + asteroidRadian) - radius * cos(radian * i + asteroidRadian);
		//offsets are used to make the unique shape of the asteroid
		//the offsets are randomly chosen when the asteroid is made, and unique from other asteroids
		x *= asteroid->offsets[i] / 10;
		y *= asteroid->offsets[i] / 10;
		glVertex3f(x + asteroid->posX, y + asteroid->posY, 0.0);
	}
	glEnd();

	//This is used to draw the circle hitbox of the asteroid
	if (SHOW_HITBOX == true)
	{
		float r = radius;
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (float i = -r; i <= r; i += 0.01f)
		{
			float circleX = i;
			float circleY = sqrt(r * r - circleX * circleX);
			glVertex3f(circleX + asteroid->posX, asteroid->posY + circleY, 1.0);
			glVertex3f(circleX + asteroid->posX, asteroid->posY - circleY, 1.0);
		}
		glEnd();
	}
}

//this function is used to draw the explosion of asteroid, or player 
void drawExplosion(Explosion * explosion)
{
	glPointSize(explosion->particleSize);
	for (auto& particle : explosion->particles)
	{
		glColor3f(particle->color[0], particle->color[1], particle->color[2]);
		glPointSize(particle->size);
		glBegin(GL_POINTS);
		glVertex3f(particle->posX, particle->posY, 0.0);
		glEnd();
	}
}

//this fucntion is used to draw the message at the start of the game
void drawStartMessage()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	std::string str = "Press any keys to start...";
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (str.size() / 2 - 2), (screenHeight - arenaHeight) / 2 + arenaHeight / 2);
	for (std::string::iterator i = str.begin(); i != str.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//this fucntion is used to draw the message when player have no life left
void drawDieMessage()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	std::string str = "Game Over. Press any key to play again...";
	glRasterPos2f((screenWidth - arenaWidth) / 2 + arenaWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (str.size() / 2 - 2), (screenHeight - arenaHeight) / 2 + arenaHeight / 2);
	for (std::string::iterator i = str.begin(); i != str.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//this function is used to display objects
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(arena_color[0], arena_color[1], arena_color[2], 0.0);
	glEnable(GL_DEPTH_TEST);
	drawArena(arenaWidth, arenaHeight);
	if(SHOW_FPS) drawFPS(std::to_string(static_cast<int>(lastFPS)));
	if (gameStarted)
	{
		if (playerDied)
		{
			drawDieMessage();
			time_since_game_start = 0;
		}
		else
		{
			drawPlayerInfo(player);
			drawPlayer(player);
			for (auto& bullet : bullets)
			{
				drawBullet(bullet);
			}
			for (auto& explosion : explosions)
			{
				drawExplosion(explosion);
			}
		}
		for (auto& asteroid : asteroids)
		{
			drawAsteroid(asteroid);
		}
		drawBlackHole(blackHoleX, blackHoleY);
	}
	else
	{
		drawStartMessage();
	}
	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "display: " << gluErrorString(err) << std::endl;
	}
	glutSwapBuffers();
}

//this function is used to recognised the keyboard input
void on_key_press(unsigned char key, int x, int y)
{
	if (gameStarted && !playerDied) //work normally when game started (not in the beginning screen) and player still have some life left
	{
		if (key == 27)
		{
			exit(EXIT_SUCCESS);
		}
		//i use the KeyState here so that it can detect multiple inputs at once 
		if (key == 'w')
		{
			keyArray[int('w')] = KeyState::PRESSED;
		}
		if (key == 'a')
		{
			keyArray[int('a')] = KeyState::PRESSED;
		}
		if (key == 'd')
		{
			keyArray[int('d')] = KeyState::PRESSED;
		}
	}
	else if(!gameStarted) //this is at the begining of the game, press any keys to start the game
	{
		if (key == 27)
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			gameStarted = true;
		}
	}
	else if (playerDied) //this is when player have no life left, press any key to restart the game
	{
		if (key == 27)
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			restartGame();
		}
	}
}

//this function is used to set the keystate of keyArray back to free when release the key
void on_key_release(unsigned char key, int x, int y)
{
	if (key == 'w')
	{
		keyArray[int('w')] = KeyState::FREE;
	}
	if (key == 'a')
	{
		keyArray[int('a')] = KeyState::FREE;
	}
	if (key == 'd')
	{
		keyArray[int('d')] = KeyState::FREE;
	}
}

//this function is used to detect the mouse input, the same as on_key_press
void on_mouse_button(int btn, int state, int x, int y)
{
	if (gameStarted  && !playerDied)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			mousePressed = KeyState::PRESSED;
		}
		else
		{
			mousePressed = KeyState::FREE;
		}
	}
	else if(!gameStarted)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			gameStarted = true;
		}
		else if(btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		{
			gameStarted = true;
		}
	}
	else if (playerDied)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			restartGame();
		}
		else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		{
			restartGame();
		}
	}
}

//this function is used to calculate the player's x, y when the player move as well as the particles coming out from the back of the ship when thrusting
void playerMovementAndParticles()
{
	//Ship movement
	if (keyArray[int('a')] == KeyState::PRESSED)
	{
		player->rotateLeft();
	}
	if (keyArray[int('d')] == KeyState::PRESSED)
	{
		player->rotateRight();
	}
	if (keyArray[int('w')] == KeyState::PRESSED)
	{
		//MAX_SPEED is to prevent the player going to fast
		if (player->velocityX <= MAX_SPEED)
		{
			player->velocityX -= player->thrustSpeed * sin(player->playerDirectionRadian) / currentFPS;
		}
		if (player->velocityY <= MAX_SPEED)
		{
			player->velocityY += player->thrustSpeed * cos(player->playerDirectionRadian) / currentFPS;
		}
		isThrusting = true; //this is used to determine when to draw the particles
		//this is used to draw the particles coming out from the back of the ship
		if (particles.size() < particleNum)
		{
			if (particle_dt >= timeBetweenParticles) //this is to delaying the particle coming out
			{
				Particle* particle = new Particle(player, INIT_PARTICLE_SIZE, particle_color);
				particles.push_back(particle);
				particle->velocityX = particleSpeed / currentFPS * -sin(particle->particleDirectionRadian);
				particle->velocityY = particleSpeed / currentFPS * cos(particle->particleDirectionRadian);
				particle_dt -= timeBetweenParticles;
			}
			else
			{
				particle_dt += dt;
			}
		}
	}
	else
	{
		isThrusting = false;
		//if player release the 'w' key, the player will slow down by FRICTION
		player->velocityX -= FRICTION * player->velocityX / currentFPS;
		player->velocityY -= FRICTION * player->velocityY / currentFPS;
	}
	player->move(); //player move() function is used to add posX with velocityX as well as posY with velocityY

	//this is used to reduce the size of particles when time passed as well as changing their color
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->move();
		particles[i]->size -= dt * sizeReduceRate; //reduce the size base on the dt
		if (particles[i]->size <= 0.0f) //when they are smaller than 0, removed it from the vector of particles
		{
			delete particles[i];
			particles.erase(particles.begin() + i);
			break;
		}
	}

	if (GO_THROUGH_WALL == true || invulnerable_state) //this is used when the mode is set to go through wall, or when player is in invulnerable state
		//which mean player cannot die if touch the wall during the invulnerable time
	{
		//this is to draw the player at the opposite site of the wall when go through the wall
		if (player->posX < ((screenWidth - arenaWidth) / 2 - player->playerSize) && player->velocityX < 0)
		{
			player->posX = arenaWidth + (screenWidth - arenaWidth) / 2 + player->playerSize * 2;
		}
		else if (player->posX > (arenaWidth + (screenWidth - arenaWidth) / 2 + player->playerSize) && player->velocityX > 0)
		{
			player->posX = (screenWidth - arenaWidth) / 2 - player->playerSize * 2;
		}
		else if (player->posY < ((screenHeight - arenaHeight) / 2 - player->playerSize) && player->velocityY < 0)
		{
			player->posY = arenaHeight + (screenHeight - arenaHeight) / 2 + player->playerSize * 2;
		}
		else if (player->posY > (arenaHeight + (screenHeight - arenaHeight) / 2 + player->playerSize) && player->velocityY > 0)
		{
			player->posY = (screenHeight - arenaHeight) / 2 - player->playerSize * 2;
		}
	}
	else
	{
		//this is used to check when player touch the wall 
		if (player->posX < (screenWidth - arenaWidth) / 2
			|| player->posX >  arenaWidth + (screenWidth - arenaWidth) / 2
			|| player->posY < (screenHeight - arenaHeight) / 2
			|| player->posY > arenaHeight + (screenHeight - arenaHeight) / 2)
		{
			if (!invulnerable_state)
			{
				explosions.push_back(new Explosion(player->posX, player->posY, player->playerSize * 2, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
				player->die();
				invulnerable_state = true;
			}
		}

		warnings.clear(); //we want to clear the warnings vector everytime, so that the wall can be set back to WHITE when player is not near the wall
		//I didnt use else here so that there can be multiple warnings at the same time
		if (player->posX < (screenWidth - arenaWidth) / 2 + WARNING_DISTANCE)
		{
			warnings.push_back(WallWarning::W);
		}
		if (player->posX > (arenaWidth + (screenWidth - arenaWidth) / 2 - WARNING_DISTANCE))
		{
			warnings.push_back(WallWarning::E);
		}
		if (player->posY < (screenHeight - arenaHeight) / 2 + WARNING_DISTANCE)
		{
			warnings.push_back(WallWarning::S);
		}
		if (player->posY > (arenaHeight + (screenHeight - arenaHeight) / 2 - WARNING_DISTANCE))
		{
			warnings.push_back(WallWarning::N);
		}
	}
	
	//check when player have no life left
	if (player->life < 0)
	{
		playerDied = true;
	}

	//this is used to manage the invulnerable state of the player via dt
	if (invulnerable_state)
	{
		after_die_time += dt;
	}
	else
	{
		after_die_time = 0;
	}
	if (after_die_time >= INVULNERABLE_TIME)
	{
		invulnerable_state = false;
	}
}

//this function is used to spawn and calculate the asteroids' movement as well as collison, explosion,.. 
void asteroidSpawnAndMovement(float dt)
{
	//this is used to limit the number of asteroids per round as well as the delaying between the spawning of 2 asteroids
	asteroid_spawn_delay += dt;
	if (asteroid_spawn_delay >= asteroid_spawn_rate && asteroids_number > 0)
	{
		asteroids_number--;
		int int_currentWidth = static_cast<int>(screenWidth);
		int int_currentHeight = static_cast<int>(screenHeight);
		int side = rand() % 2;
		float asteroidX, asteroidY;
		if (side == 0)
		{
			asteroidX = (float)(rand() % 100 + ((int_currentWidth - (int)arenaWidth) / 2 + arenaWidth + 100));
		}
		else
		{
			asteroidX = (float)(rand() % 100 + ((int_currentWidth - (int)arenaWidth) / 2 - 200));
		}
		side = rand() % 2;
		if (side == 0)
		{
			asteroidY = (float)(rand() % 100 + ((int_currentHeight - (int)arenaHeight) / 2 + arenaHeight + 100));
		}
		else
		{
			asteroidY = (float)(rand() % 100 + ((int_currentHeight - (int)arenaHeight) / 2 - 200));
		}
		float asteroid_radius = (float)(rand() % 50 + 50);
		Asteroid* asteroid = new Asteroid(asteroidX, asteroidY, asteroid_radius, player->posX, player->posY, AsteroidState::SPLITTABLE);
		asteroid->velocityX = asteroid->asteroidSpeed * -sin(asteroid->asteroidDirectionRadian) / currentFPS;
		asteroid->velocityY = asteroid->asteroidSpeed * cos(asteroid->asteroidDirectionRadian) / currentFPS;
		asteroids.push_back(asteroid);
		asteroid_spawn_delay = 0.0;
	}

	//if the asteroids_number is 0 and the size of the vector is 0, go to next rounds with new asteroids_number 
	if (asteroids_number == 0 && asteroids.size() == 0)
	{
		currRound++;
		asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
	}

	//this is used to calculate the movement of the asteroids when touching the wall or touching each other
	for (auto& asteroid : asteroids)
	{
		//when touching the wall, just need to change the side of the velocity
		if (asteroid->posX > arenaWidth + (screenWidth - arenaWidth) / 2 - asteroid->radius && asteroid->velocityX > 0)
		{
			asteroid->velocityX *= -1;
		}
		else if (asteroid->posX < (screenWidth - arenaWidth) / 2 + asteroid->radius && asteroid->velocityX < 0)
		{
			asteroid->velocityX *= -1;
		}
		else if (asteroid->posY > arenaHeight + (screenHeight - arenaHeight) / 2 - asteroid->radius && asteroid->velocityY > 0)
		{
			asteroid->velocityY *= -1;
		}
		else if (asteroid->posY < (screenHeight - arenaHeight) / 2 + asteroid->radius && asteroid->velocityY < 0)
		{
			asteroid->velocityY *= -1;
		}
		//this is used to calculate the movement of 2 asteroids when they touch each other, base on elastic collison
		for (auto& asteroid2 : asteroids)
		{
			if (asteroid2 != asteroid)
			{
				float d = distance(asteroid->posX, asteroid->posY, asteroid2->posX, asteroid2->posY);
				if (d <= asteroid2->radius + asteroid->radius)
				{
					float overlap = 0.5f * (d - asteroid->radius - asteroid2->radius);
					asteroid->posX -= overlap * (asteroid->posX - asteroid2->posX) / d;
					asteroid->posY -= overlap * (asteroid->posY - asteroid2->posY) / d;
					asteroid2->posX += overlap * (asteroid->posX - asteroid2->posX) / d;
					asteroid2->posY += overlap * (asteroid->posY - asteroid2->posY) / d;
					float nx = (asteroid->posX - asteroid2->posX) / d;
					float ny = (asteroid->posY - asteroid2->posY) / d;
					float tx = -ny;
					float ty = nx;
					float dpT1 = asteroid->velocityX * tx + asteroid->velocityY * ty;
					float dpT2 = asteroid2->velocityX * tx + asteroid2->velocityY * ty;
					float dpN1 = asteroid->velocityX * nx + asteroid->velocityY * ny;
					float dpN2 = asteroid2->velocityX * nx + asteroid2->velocityY * ny;
					float m1 = (dpN1 * (asteroid->mass - asteroid2->mass) + 2.0f * asteroid2->mass * dpN2) / (asteroid->mass + asteroid2->mass);
					float m2 = (dpN2 * (asteroid2->mass - asteroid->mass) + 2.0f * asteroid->mass * dpN1) / (asteroid->mass + asteroid2->mass);
					asteroid->velocityX = tx * dpT1 + nx * m1;
					asteroid->velocityY = ty * dpT1 + ny * m1;
					asteroid2->velocityX = tx * dpT2 + nx * m2;
					asteroid2->velocityY = ty * dpT2 + ny * m2;
				}
			}
		}
		//asteroid rotate() is used to make the asteroid spinning, the spinning speed and direction is randomly chosen when the asteroid is made
		asteroid->rotate();

		//this is used to apply the friction onto the asteroid
		asteroid->velocityX -= ASTEROID_FRICTION * asteroid->velocityX / currentFPS;
		asteroid->velocityY -= ASTEROID_FRICTION * asteroid->velocityY / currentFPS;

		//this is used to prevent the asteroid going crazy 
		if (asteroid->velocityX > ASTEROID_MAX_VELOCITY)
		{
			asteroid->velocityX = ASTEROID_MAX_VELOCITY;
		}
		if (asteroid->velocityY > ASTEROID_MAX_VELOCITY)
		{
			asteroid->velocityY = ASTEROID_MAX_VELOCITY;
		}
		//add velocity to position
		asteroid->move();
	}

	//this is used to check if any asteroids touch the player by check the distance
	for (int i = 0; i < asteroids.size(); i++)
	{
		if (distance(player->posX, player->posY, asteroids[i]->posX, asteroids[i]->posY) <= (asteroids[i]->radius + player->playerSize))
		{
			if (!invulnerable_state)
			{
				explosions.push_back(new Explosion(player->posX, player->posY, player->playerSize * 2, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
				player->die();
				invulnerable_state = true;
			}
		}
	}

	//this is calculate the splitting effect and 2 small asteroids' movement after being splitted from the big asteroid
	for (int j = 0; j < asteroids.size(); j++)
	{
		if (asteroids[j]->hp <= 0) //the asteroid only destroyed when its hp below 0
		{
			player->score += (int)asteroids[j]->radius; //increase the score of the player by the radius of asteroid
			player->asteroidsDestroyed++;

			if (asteroids[j]->state == AsteroidState::SPLITTABLE) //asteroid can only be splitted once, so is this used to check if the asteroid is splittable
			{
				float r1 = asteroids[j]->asteroidDirectionRadian - PI * 0.5f;
				float r2 = asteroids[j]->asteroidDirectionRadian + PI * 0.5f;
				float x1 = asteroids[j]->posX - sin(r1) * asteroids[j]->radius;
				float y1 = asteroids[j]->posY + cos(r1) * asteroids[j]->radius;
				float x2 = asteroids[j]->posX - sin(r2) * asteroids[j]->radius;
				float y2 = asteroids[j]->posY + cos(r2) * asteroids[j]->radius;
				//make 2 asteroid that is unsplittable
				Asteroid* a1 = new Asteroid(x1, y1, asteroids[j]->radius * 0.5f, 0, 0, AsteroidState::UNSPLITTABLE);
				Asteroid* a2 = new Asteroid(x2, y2, asteroids[j]->radius * 0.5f, 0, 0, AsteroidState::UNSPLITTABLE);
				a1->asteroidDirectionRadian = asteroids[j]->asteroidDirectionRadian - PI * 0.25f; // 45 degree to the left
				a2->asteroidDirectionRadian = asteroids[j]->asteroidDirectionRadian + PI * 0.25f; // 45 degree to the right
				//velocity of 2 new asteroids is the same as the big asteroid, but change in direction
				a1->velocityX = asteroids[j]->velocityX / sin(asteroids[j]->asteroidDirectionRadian) * sin(a1->asteroidDirectionRadian); 
				a1->velocityY = asteroids[j]->velocityY / cos(asteroids[j]->asteroidDirectionRadian) * cos(a1->asteroidDirectionRadian);
				a2->velocityX = asteroids[j]->velocityX / sin(asteroids[j]->asteroidDirectionRadian) * sin(a2->asteroidDirectionRadian);
				a2->velocityY = asteroids[j]->velocityY / cos(asteroids[j]->asteroidDirectionRadian) * cos(a2->asteroidDirectionRadian);
				asteroids.push_back(a1);
				asteroids.push_back(a2);
			}
			//make an explosion at the big asteroid location
			explosions.push_back(new Explosion(asteroids[j]->posX, asteroids[j]->posY, asteroids[j]->radius, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
			delete asteroids[j];
			asteroids.erase(asteroids.begin() + j);
			break;
		}
	}
}

//this function is used to spawn and calculate the bullets' movement and collision
void bulletSpawnAndMovement(float dt)
{
	//this is used to limit the firing rate  
	shoot_delay += dt;
	if (shoot_delay >= TIME_DELAY_BETWEEN_TWO_SHOTS)
	{
		shoot_delay = 0.0f;
		shootable = true;
	}

	if (mousePressed == KeyState::PRESSED && shootable == true)
	{
		shootable = false;
		shoot_delay = 0.0f;
		float size = player->playerSize;
		float radian = player->playerDirectionRadian;
		float posX = player->posX;
		float posY = player->posY;
		posX += -size * 1.5f * sin(radian);
		posY += size * 1.5f * cos(radian);
		Bullet* bullet = new Bullet(posX, posY, radian, BULLET_SIZE);
		bullet->velocityX = -sin(bullet->bulletDirectionRadian) * BULLET_SPEED / currentFPS;
		bullet->velocityY = cos(bullet->bulletDirectionRadian) * BULLET_SPEED / currentFPS;
		bullets.push_back(bullet);
	}

	//delete the bullet that are out of the wall
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->posX > arenaWidth + (screenWidth - arenaWidth) / 2
			|| bullets[i]->posX < (screenWidth - arenaWidth) / 2
			|| bullets[i]->posY >  arenaHeight + (screenHeight - arenaHeight) / 2
			|| bullets[i]->posY < (screenHeight - arenaHeight) / 2)
		{
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
			break;
		}
	}

	//delete the bullet that hit the asteroid
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->move();
		for (auto& asteroid : asteroids)
		{
			if (distance(bullets[i]->posX, bullets[i]->posY, asteroid->posX, asteroid->posY) <= (asteroid->radius + BULLET_SIZE))
			{
				asteroid->getHit();
				//asteroid only get hit and lose hp here, asteroid will be deleted when its hp is 0
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				break;
			}
		}
	}
}

//this function is used to manage the particles in the explosion
void explosionEffect(float dt)
{
	for (int i = 0; i < explosions.size(); i++)
	{
		for (int j = 0; j < explosions[i]->particles.size(); j++)
		{
			explosions[i]->particles[j]->move();
			explosions[i]->particles[j]->size -= dt * explosions[i]->sizeReduceRate; //make the particle smaller with dt
			if (explosions[i]->particles[j]->size <= 0.0) //if the particle is smaller than 0, it will be deleted
			{
				delete explosions[i]->particles[j];
				explosions[i]->particles.erase(explosions[i]->particles.begin() + j);
				break;
			}
		}
		if (explosions[i]->particles.size() == 0) //when all the particles are deleted, the explosion will be deleted and removed from the vector
		{
			delete explosions[i];
			explosions.erase(explosions.begin() + i);
			break;
		}
	}
}

//this function is used to make a pulling effect of the black hole
void blackHolePulling(float dt)
{
	if (blackHoleActivated) //check if the black hole is activated or on cooldown
	{
		//change the movement of the bullet 
		for (int i = 0; i < bullets.size(); i++)
		{
			if (distance(bullets[i]->posX, bullets[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS)
			{
				float forceX = blackHoleX - bullets[i]->posX;
				float forceY = blackHoleY - bullets[i]->posY;
				float r = sqrt(forceX * forceX + forceY * forceY);
				//base on Rs = 2 * G * M / c^2 with c is speed of light, M is mass, G  is gracitational constant
				float fg = (2.0f * 0.6f * BL_MASS) / (r * r);
				forceX *= fg / r;
				forceY *= fg / r;
				bullets[i]->velocityX += forceX;
				bullets[i]->velocityY += forceY;
				float r2 = sqrt(bullets[i]->velocityX * bullets[i]->velocityX + bullets[i]->velocityY * bullets[i]->velocityY);
				bullets[i]->velocityX *= 3.0f / r2;
				bullets[i]->velocityY *= 3.0f / r2;
			}
			if (distance(bullets[i]->posX, bullets[i]->posY, blackHoleX, blackHoleY) <= 10.0f)
			{
				bullets[i]->size -= blackHolePullingRate; //make the effect that the bullet is sucked into the black hole, and get smaller by time
				if (bullets[i]->size <= 0)
				{
					delete bullets[i];
					bullets.erase(bullets.begin() + i);
					break;
				}
			}
		}
		//change the movement of the asteroid 
		for (int i = 0; i < asteroids.size(); i++)
		{
			if (distance(asteroids[i]->posX, asteroids[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS)
			{
				float forceX = blackHoleX - asteroids[i]->posX;
				float forceY = blackHoleY - asteroids[i]->posY;
				float r = sqrt(forceX * forceX + forceY * forceY);
				//base on Rs = 2 * G * M / c^2 with c is speed of light, M is mass, G  is gracitational constant
				float fg = (2.0f * 0.6f * BL_MASS) / (r * r);
				forceX *= fg / r;
				forceY *= fg / r;
				asteroids[i]->velocityX += forceX;
				asteroids[i]->velocityY += forceY;
				float r2 = sqrt(asteroids[i]->velocityX * asteroids[i]->velocityX + asteroids[i]->velocityY * asteroids[i]->velocityY);
				asteroids[i]->velocityX *= 3.0f / r2;
				asteroids[i]->velocityY *= 3.0f / r2;
			}
			if (distance(asteroids[i]->posX, asteroids[i]->posY, blackHoleX, blackHoleY) <= 10.0f)
			{
				asteroids[i]->radius -= blackHolePullingRate; //make the effect that the asteroid is sucked into the black hole, and get smaller by time
				if (asteroids[i]->radius <= 0)
				{
					delete asteroids[i];
					asteroids.erase(asteroids.begin() + i);
					break;
				}
			}
		}
		//change the movement of the particles
		for (int i = 0; i < particles.size(); i++)
		{
			if (distance(particles[i]->posX, particles[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS)
			{
				float forceX = blackHoleX - particles[i]->posX;
				float forceY = blackHoleY - particles[i]->posY;
				float r = sqrt(forceX * forceX + forceY * forceY);
				//base on Rs = 2 * G * M / c^2 with c is speed of light, M is mass, G  is gracitational constant
				float fg = (2.0f * 0.6f * BL_MASS) / (r * r);
				forceX *= fg / r;
				forceY *= fg / r;
				particles[i]->velocityX += forceX;
				particles[i]->velocityY += forceY;
				float r2 = sqrt(particles[i]->velocityX * particles[i]->velocityX + particles[i]->velocityY * particles[i]->velocityY);
				particles[i]->velocityX *= 3.0f / r2;
				particles[i]->velocityY *= 3.0f / r2;
			}
			if (distance(particles[i]->posX, particles[i]->posY, blackHoleX, blackHoleY) <= 10.0f)
			{
				particles[i]->radius -= blackHolePullingRate;//make the effect that the particle is sucked into the black hole, and get smaller by time
				if (particles[i]->radius <= 0)
				{
					delete particles[i];
					particles.erase(particles.begin() + i);
					break;
				}
			}
		}
		//change the movement of the player
		if (!invulnerable_state)
		{
			if (distance(player->posX, player->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS)
			{
				float forceX = blackHoleX - player->posX;
				float forceY = blackHoleY - player->posY;
				float r = sqrt(forceX * forceX + forceY * forceY);
				//base on Rs = 2 * G * M / c^2 with c is speed of light, M is mass, G  is gracitational constant
				float fg = (2.0f * 0.6f * BL_MASS) / (r * r);
				forceX *= fg / r;
				forceY *= fg / r;
				player->velocityX += forceX;
				player->velocityY += forceY;
				float r2 = sqrt(player->velocityX * player->velocityX + player->velocityY * player->velocityY);
				player->velocityX *= 3.0f / r2;
				player->velocityY *= 3.0f / r2;
			}
			if (distance(player->posX, player->posY, blackHoleX, blackHoleY) <= 10.0f)
			{
				player->playerSize -= blackHolePullingRate;//make the effect that the player is sucked into the black hole, and get smaller by time
				if (player->playerSize <= 0)
				{
					player->die();
					invulnerable_state = true;
				}
			}
		}
	}

	//this is used to manage the frequency of the black hole
	bh_dt += dt;
	if (bh_dt >= blackHoleFrequency)
	{
		bh_dt = 0.0f;
		pulseVel = 0.0f;
		if (blackHoleActivated)
		{
			blackHoleActivated = false;
			player->playerSize = PLAYER_SIZE;
			//just to make sure player wont shrink after surviving the pulling of the black hole
		}
		else
		{
			blackHoleActivated = true;
		}
	}
}

//this function is used to manage other object state function
void update_game_state(float dt)
{
	playerMovementAndParticles();
	asteroidSpawnAndMovement(dt);
	bulletSpawnAndMovement(dt);
	explosionEffect(dt);
	blackHolePulling(dt);
}

//idle function
void idle()
{
	float t = (float)glutGet(GLUT_ELAPSED_TIME);
	dt = t - last_t;
	if (gameStarted)
	{
		time_since_game_start += dt;
		update_game_state(dt);
	}
	last_t = t;
	//FPS update delay time
	currentFPS = 1000 / dt;
	fps_update_delay += dt;
	if (fps_update_delay >= FPS_UPDATE_DELAY) //2 seconds
	{
		fps_update_delay = 0.0f;
		lastFPS = currentFPS;
	}
	glutPostRedisplay();
}

//app init function
void init(float initScreenWidth, float initScreenHeight)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, initScreenWidth, 0, initScreenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	screenWidth = (float)glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = (float)glutGet(GLUT_SCREEN_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	

	glutCreateWindow("Assignment 1");

	//base on the mode, the arena can fit the screen or can be adjusted manually
	if (ARENA_FIT_SCREEN)
	{
		arenaWidth = screenWidth;
		arenaHeight = screenHeight;
	}
	else
	{
		if (arenaWidth > screenWidth)
		{
			arenaWidth = screenWidth;
		}
		if (arenaHeight > screenHeight)
		{
			arenaHeight = screenHeight;
		}
	}
	init(screenWidth, screenHeight);
	glutFullScreen();
	srand(time(NULL));

	//random black hole positions
	blackHoleX = (rand() % (int)(arenaWidth - 2 * BLACK_HOLE_RADIUS)) + (screenWidth - arenaWidth) / 2 + BLACK_HOLE_RADIUS;
	blackHoleY = (rand() % (int)(arenaHeight - 2 * BLACK_HOLE_RADIUS)) + (screenHeight - arenaHeight) / 2 + BLACK_HOLE_RADIUS;

	//base on the mode that player can be spawn in the middle or can be adjusted manually
	if (SPAWNING_IN_MIDDLE)
	{
		player = new Player(screenWidth / 2, screenHeight / 2);
	}
	else
	{
		player = new Player((screenWidth - arenaWidth)/2 + SPAWNING_POS, (screenHeight - arenaHeight) / 2 + SPAWNING_POS);
	}

	//add mouse function
	glutMouseFunc(on_mouse_button);

	//add key function 
	glutKeyboardFunc(on_key_press);
	glutKeyboardUpFunc(on_key_release);

	//add idle function
	glutIdleFunc(idle);

	//add display
	glutDisplayFunc(display);

	glutMainLoop();

	return EXIT_SUCCESS;
}

