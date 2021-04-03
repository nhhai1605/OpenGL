#include "Player.h"
#include "Bullet.h"
#include "Utility.h"
#include "Asteroid.h"
#include "Particle.h"
#include "Explosion.h"
#if _WIN32
#   include <Windows.h>
#else
#include <unistd.h>
#endif

#if __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#   include <GLUT/glut.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif

//true for seeing the objects' hitbox
#define SHOW_HITBOX true

//false to activate the "touch wall then die" mode
#define GO_THROUGH_WALL false

//true to spawning in the middle, false to set it manually, position will base on the arena width and height, not screen width and height
#define SPAWNING_IN_MIDDLE true
#define SPAWNING_POS 200.0f

#define SHOW_FPS true

//true to let the arena fit the screen, fasle to set it manually
#define ARENA_FIT_SCREEN true
float arenaWidth = 800, arenaHeight = 400;

//Player color
float player_outline_color[] = { 1.0, 0.0, 0.0 };
float player_fill_color[] = {1.0, 1.0, 1.0};

//Boost particle
int particleNum = 200;
float decayTime = 2000.0f; // ms
float sizeReduceRate = INIT_PARTICLE_SIZE / decayTime; //per ms
float particle_color[] = { 1.0, 0.2, 0.02};
float particleSpeed = 200.0f;
float timeBetweenParticles = 10.0f;
float particle_dt = 0.0f;

//explosion
int explosionParticles = 200;

//Bullet color
float bullet_color[] = { 1.0, 0.0, 0.0 };

//arenacolor
float arena_color[] = { 0.0f, 0.0f, 0.0f};

//wall warning color
float warning_color[] = { 1.0, 0.0, 0.0 };

//change number of init asteroids here
int currRound = 1;
int asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
int asteroid_spawn_rate = 2000;
Player* player;
std::vector <Bullet*> bullets;
std::vector <Asteroid*> asteroids;
KeyState keyArray[127], mousePressed;
std::vector <WallWarning> warnings;
std::vector <Particle*> particles;
std::vector <Explosion*> explosions;
//Global variables
int screenWidth, screenHeight;
float currentFPS = 0.0f, lastFPS = 0.0f;
float last_t = 0.0f, dt = 0.0f;
float shoot_delay = 0.0f, fps_update_delay = 0.0f, asteroid_spawn_delay = 0.0f;
bool shootable = true;
bool isThrusting = false;
bool FPSupdatable = true;
bool gameStarted = false;
bool playerDied = false;
int time_since_game_start = 0;
int after_die_time = 0;
bool invulnerable_state = false;
int blinking_num = INVULNERABLE_TIME / BLINKING_DUR;	
float blinking_time = BLINKING_DUR;
float blackHoleX = 0.0f, blackHoleY = 0.0f;

float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void restartGame()
{
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

void drawBlackHole(float posX, float posY)
{
	glColor3f(0.8, 0.3, 0.8);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (float i = -BLACK_HOLE_RADIUS; i <= BLACK_HOLE_RADIUS; i += 0.1)
	{
		float circleX = i;
		float circleY = sqrt(BLACK_HOLE_RADIUS * BLACK_HOLE_RADIUS - circleX * circleX);
		glColor3f(0.8, 0.3, 0.8);
		glVertex3f(circleX + posX, posY + circleY, -0.5);
		glVertex3f(circleX + posX, posY - circleY, -0.5);
	}
	glEnd();
	int space = 5;
	glBegin(GL_POINTS);
	glColor3f(0.5, 0.1, 0.5);
	for (int i = 0; i < 300; i++)
	{
		float angle = 0.1 * i;
		float r = glutGet(GLUT_ELAPSED_TIME) / 40;
		float x = (space * angle) * cos(angle - r);
		float y = (space * angle) * sin(angle - r);
		glVertex3f(x + posX, posY + y, -0.5);
	}
	glEnd();

}


void drawArena(int arenaWidth, int arenaHeight)
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
	float leftCornerX = (screenWidth - arenaWidth) / 2;
	float leftCornerY = (screenHeight - arenaHeight) / 2;
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

	float t = time_since_game_start;
	int minutes = floor(t / 60000);
	int seconds = floor((t - minutes * 60000) / 1000);
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
	x2 = -size * 1.5 * sin(radian);
	y2 = size * 1.5 * cos(radian);
	x3 = -size * cos(radian) + size * sin(radian);
	y3 = -size * sin(radian) - size * cos(radian);
	
	if (invulnerable_state)
	{
		//Draw the ship	
		if (blinking_num % 2 == 0)
		{
			glColor3f(player_fill_color[0], player_fill_color[1], player_fill_color[2]);
		}
		else
		{
			glColor3f(arena_color[0], arena_color[1], arena_color[2]);
		}
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
		glLineWidth(player->playerSize / 6);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glVertex3f(x, y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glEnd();

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
		//Draw the ship	
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
	for (auto& particle : particles)
	{
		glPointSize(particle->size);
		glColor3f(particle->color[0], particle->color[1], particle->color[2]);
		glBegin(GL_POINTS);
		glVertex3f(particle->posX, particle->posY, 0.0);
		glEnd();
	}

	if (SHOW_HITBOX == true)
	{
		float r = player->playerSize;
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (float i = -r; i <= r; i += 0.01)
		{
			float circleX = i;
			float circleY = sqrt(r * r - circleX * circleX);
			glVertex3f(circleX + player->posX, player->posY + circleY, 1.0);
			glVertex3f(circleX + player->posX, player->posY - circleY, 1.0);
		}
		glEnd();
	}
}

void drawBullet(Bullet * bullet)
{
	glPointSize(bullet->size);
	glColor3f(bullet_color[0], bullet_color[1], bullet_color[2]);
	glBegin(GL_POINTS);
	glVertex3f(bullet->posX, bullet->posY, 0.0);
	glEnd();
}

void drawAsteroid(Asteroid* asteroid)
{
	glColor3f(0.5, 0.5, 0.5);
	float angle = 360 / asteroid->vertices;
	float asteroidRadian = asteroid->asteroidRotateRadian;
	float radius = asteroid->radius;
	glBegin(GL_POLYGON);
	float radian = angle * PI / 180;
	for (int i = 0; i < asteroid->vertices; i++)
	{
		float x = radius * cos(radian * i + asteroidRadian) + radius * sin(radian * i + asteroidRadian);
		float y = radius * sin(radian * i + asteroidRadian) - radius * cos(radian * i + asteroidRadian);
		x *= asteroid->offsets[i] / 10;
		y *= asteroid->offsets[i] / 10;
		glVertex3f(x + asteroid->posX, y + asteroid->posY, 0.0);
	}
	glEnd();
	if (SHOW_HITBOX == true)
	{
		float r = radius;
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (float i = -r; i <= r; i += 0.01)
		{
			float circleX = i;
			float circleY = sqrt(r * r - circleX * circleX);
			glVertex3f(circleX + asteroid->posX, asteroid->posY + circleY, 1.0);
			glVertex3f(circleX + asteroid->posX, asteroid->posY - circleY, 1.0);
		}
		glEnd();
	}
}

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

void drawFPS(std::string fps)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, 0 , screenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f((screenWidth - arenaWidth)/2 + 10, (screenHeight - arenaHeight)/2 + 10);
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

void on_key_press(unsigned char key, int x, int y)
{
	if (gameStarted && !playerDied)
	{
		if (key == 27)
		{
			exit(EXIT_SUCCESS);
		}
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
		if (key == 't')
		{
			explosions.push_back(new Explosion(player->posX, player->posY, player->playerSize * 3, particle_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
		}
	
	}
	else if(!gameStarted)
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
	else if (playerDied)
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

	glutPostRedisplay();
}

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
		if (player->velocityX <= MAX_SPEED)
		{
			player->velocityX -= player->thrustSpeed * sin(player->playerDirectionRadian) / currentFPS;
		}
		if (player->velocityY <= MAX_SPEED)
		{
			player->velocityY += player->thrustSpeed * cos(player->playerDirectionRadian) / currentFPS;
		}
		isThrusting = true;
		if (particles.size() < particleNum)
		{
			if (particle_dt >= timeBetweenParticles)
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
		player->velocityX -= FRICTION * player->velocityX / currentFPS;
		player->velocityY -= FRICTION * player->velocityY / currentFPS;
	}
	player->move();

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->move();
		particles[i]->size -= dt * sizeReduceRate;
		if (particles[i]->size <= 0.0)
		{
			delete particles[i];
			particles.erase(particles.begin() + i);
			break;
		}
		if (particles[i]->posX < (screenWidth - arenaWidth) / 2
			|| particles[i]->posX >  arenaWidth + (screenWidth - arenaWidth) / 2
			|| particles[i]->posY < (screenHeight - arenaHeight) / 2
			|| particles[i]->posY > arenaHeight + (screenHeight - arenaHeight) / 2)
		{
			delete particles[i];
			particles.erase(particles.begin() + i);
			break;
		}
	}

	if (GO_THROUGH_WALL == true || invulnerable_state)
	{
		//ship go through wall situation
		if (player->posX < ((screenWidth - arenaWidth) / 2 - player->playerSize) && player->velocityX < 0)
		{
			player->posX = arenaWidth + (screenWidth - arenaWidth) / 2 + player->playerSize * 2;
		}
		else if (player->posX > (arenaWidth + (screenWidth - arenaWidth) / 2 + player->playerSize) && player->velocityX > 0)
		{
			player->posX = (screenWidth - arenaWidth) / 2 - player->playerSize * 2;
		}

		if (player->posY < ((screenHeight - arenaHeight) / 2 - player->playerSize) && player->velocityY < 0)
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
		if (player->posX < (screenWidth - arenaWidth) / 2
			|| player->posX >  arenaWidth + (screenWidth - arenaWidth) / 2
			|| player->posY < (screenHeight - arenaHeight) / 2
			|| player->posY > arenaHeight + (screenHeight - arenaHeight) / 2)
		{
			if (!invulnerable_state)
			{
				player->die();
				invulnerable_state = true;
			}
		}
		warnings.clear();
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
	if (player->life < 0)
	{
		playerDied = true;
	}
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

void asteroidSpawnAndMovement(float dt)
{
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
			asteroidX = rand() % 100 + ((int_currentWidth - (int)arenaWidth) / 2 + arenaWidth + 100);
		}
		else
		{
			asteroidX = rand() % 100 + ((int_currentWidth - (int)arenaWidth) / 2 - 200);
		}
		side = rand() % 2;
		if (side == 0)
		{
			asteroidY = rand() % 100 + ((int_currentHeight - (int)arenaHeight) / 2 + arenaHeight + 100);
		}
		else
		{
			asteroidY = rand() % 100 + ((int_currentHeight - (int)arenaHeight) / 2 - 200);
		}
		int asteroid_radius = rand() % 50 + 50;
		Asteroid* asteroid = new Asteroid(asteroidX, asteroidY, asteroid_radius, player->posX, player->posY, AsteroidState::NOTSPLITED);
		asteroid->velocityX = asteroid->asteroidSpeed * -sin(asteroid->asteroidDirectionRadian) / currentFPS;
		asteroid->velocityY = asteroid->asteroidSpeed * cos(asteroid->asteroidDirectionRadian) / currentFPS;
		asteroids.push_back(asteroid);
		asteroid_spawn_delay = 0.0;
	}
	if (asteroids_number == 0 && asteroids.size() == 0)
	{
		currRound++;
		asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
	}
	for (auto& asteroid : asteroids)
	{
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
		if (abs(asteroid->velocityX * asteroid->velocityX + asteroid->velocityY * asteroid->velocityY) < 0.01f)
		{
			asteroid->velocityX = 0.0f;
			asteroid->velocityY = 0.0f;
		}
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
		asteroid->rotate();
		asteroid->velocityX -= ASTEROID_FRICTION * asteroid->velocityX / currentFPS;
		asteroid->velocityY -= ASTEROID_FRICTION * asteroid->velocityY / currentFPS;
		if (asteroid->velocityX > ASTEROID_MAX_VELOCITY)
		{
			asteroid->velocityX = ASTEROID_MAX_VELOCITY;
		}
		if (asteroid->velocityY > ASTEROID_MAX_VELOCITY)
		{
			asteroid->velocityY = ASTEROID_MAX_VELOCITY;
		}
		asteroid->move();
	}
	for (int i = 0; i < asteroids.size(); i++)
	{
		if (distance(player->posX, player->posY, asteroids[i]->posX, asteroids[i]->posY) <= (asteroids[i]->radius + player->playerSize))
		{
			if (!invulnerable_state)
			{
				player->die();
				invulnerable_state = true;
			}
		}
	}
	for (int j = 0; j < asteroids.size(); j++)
	{
		if (asteroids[j]->hp <= 0)
		{
			player->score += asteroids[j]->radius;
			player->asteroidsDestroyed++;
			if (asteroids[j]->state == AsteroidState::NOTSPLITED)
			{
				float r1 = asteroids[j]->asteroidDirectionRadian - PI * 0.5f;
				float r2 = asteroids[j]->asteroidDirectionRadian + PI * 0.5f;
				float x1 = asteroids[j]->posX - sin(r1) * asteroids[j]->radius;
				float y1 = asteroids[j]->posY + cos(r1) * asteroids[j]->radius;
				float x2 = asteroids[j]->posX - sin(r2) * asteroids[j]->radius;
				float y2 = asteroids[j]->posY + cos(r2) * asteroids[j]->radius;
				Asteroid* a1 = new Asteroid(x1, y1, asteroids[j]->radius * 0.5f, 0, 0, AsteroidState::SPLITED);
				Asteroid* a2 = new Asteroid(x2, y2, asteroids[j]->radius * 0.5f, 0, 0, AsteroidState::SPLITED);
				a1->asteroidDirectionRadian = asteroids[j]->asteroidDirectionRadian - PI * 0.25f;
				a2->asteroidDirectionRadian = asteroids[j]->asteroidDirectionRadian + PI * 0.25f;
				a1->velocityX = asteroids[j]->velocityX / sin(asteroids[j]->asteroidDirectionRadian) * sin(a1->asteroidDirectionRadian);
				a1->velocityY = asteroids[j]->velocityY / cos(asteroids[j]->asteroidDirectionRadian) * cos(a1->asteroidDirectionRadian);
				a2->velocityX = asteroids[j]->velocityX / sin(asteroids[j]->asteroidDirectionRadian) * sin(a2->asteroidDirectionRadian);
				a2->velocityY = asteroids[j]->velocityY / cos(asteroids[j]->asteroidDirectionRadian) * cos(a2->asteroidDirectionRadian);
				asteroids.push_back(a1);
				asteroids.push_back(a2);
			}
			explosions.push_back(new Explosion(asteroids[j]->posX, asteroids[j]->posY, asteroids[j]->radius, particle_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
			delete asteroids[j];
			asteroids.erase(asteroids.begin() + j);
			break;
		}
	}
}

void bulletSpawnAndMovement(float dt)
{
	shoot_delay += dt;
	if (shoot_delay >= TIME_DELAY_BETWEEN_TWO_SHOTS)
	{
		shoot_delay = 0.0f;
		shootable = true;
	}

	//Shooting
	if (mousePressed == KeyState::PRESSED && shootable == true)
	{
		shootable = false;
		shoot_delay = 0.0f;
		float size = player->playerSize;
		float radian = player->playerDirectionRadian;
		float posX = player->posX;
		float posY = player->posY;
		posX += -size * 1.5 * sin(radian);
		posY += size * 1.5 * cos(radian);
		Bullet* bullet = new Bullet(posX, posY, radian, BULLET_SIZE);
		bullet->velocityX = -sin(bullet->bulletDirectionRadian) * BULLET_SPEED / currentFPS;
		bullet->velocityY = cos(bullet->bulletDirectionRadian) * BULLET_SPEED / currentFPS;
		bullets.push_back(bullet);
	}

	//delete bullet that are out of the wall
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

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->move();
		for (auto& asteroid : asteroids)
		{
			if (distance(bullets[i]->posX, bullets[i]->posY, asteroid->posX, asteroid->posY) <= (asteroid->radius + BULLET_SIZE))
			{
				asteroid->getHit();
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				break;
			}
		}
	}
}

void explosionEffect(float dt)
{
	for (int i = 0; i < explosions.size(); i++)
	{
		for (int j = 0; j < explosions[i]->particles.size(); j++)
		{
			explosions[i]->particles[j]->move();
			explosions[i]->particles[j]->size -= dt * explosions[i]->sizeReduceRate;
			if (explosions[i]->particles[j]->size <= 0.0)
			{
				delete explosions[i]->particles[j];
				explosions[i]->particles.erase(explosions[i]->particles.begin() + j);
				break;
			}
		}
		if (explosions[i]->particles.size() == 0)
		{
			delete explosions[i];
			explosions.erase(explosions.begin() + i);
			break;
		}
	}
}

void blackHolePulling(float dt)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (distance(bullets[i]->posX, bullets[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS + BULLET_SIZE)
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
			bullets[i]->size -= 5.0f;
			if (bullets[i]->size <= 0)
			{
				delete bullets[i];
				bullets.erase(bullets.begin() + i);
				break;
			}
		}
	}

	for (int i = 0; i < asteroids.size(); i++)
	{
		if (distance(asteroids[i]->posX, asteroids[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS + asteroids[i]->radius)
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
			asteroids[i]->radius-= 5.0f;
			if (asteroids[i]->radius <= 0)
			{
				delete asteroids[i];
				asteroids.erase(asteroids.begin() + i);
				break;
			}
		}
	}

	for (int i = 0; i < particles.size(); i++)
	{
		if (distance(particles[i]->posX, particles[i]->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS + particles[i]->radius)
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
		if (distance(particles[i]->posX, particles[i]->posY, blackHoleX, blackHoleY) <= 10.0f )
		{
			particles[i]->radius -= 5.0f;
			if (particles[i]->radius <= 0)
			{
				delete particles[i];
				particles.erase(particles.begin() + i);
				break;
			}
		}
	}

	if (distance(player->posX, player->posY, blackHoleX, blackHoleY) <= BLACK_HOLE_RADIUS + player->playerSize)
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
		player->playerSize -= 5.0f;
		if (player->playerSize <= 0)
		{
			player->die();
			invulnerable_state = true;
		}
	}
}

void update_game_state(float dt)
{
	playerMovementAndParticles();
	asteroidSpawnAndMovement(dt);
	bulletSpawnAndMovement(dt);
	explosionEffect(dt);
	blackHolePulling(dt);
}

void idle()
{
	float t = glutGet(GLUT_ELAPSED_TIME);
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

void init(int initScreenWidth, int initScreenHeight)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, initScreenWidth, 0, initScreenHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	

	glutCreateWindow("Assignment 1");
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
	blackHoleX = (rand() % (int)(arenaWidth - 2 * BLACK_HOLE_RADIUS)) + (screenWidth - arenaWidth) / 2 + BLACK_HOLE_RADIUS;
	blackHoleY = (rand() % (int)(arenaHeight - 2 * BLACK_HOLE_RADIUS)) + (screenHeight - arenaHeight) / 2 + BLACK_HOLE_RADIUS;
	if (SPAWNING_IN_MIDDLE)
	{
		player = new Player(screenWidth / 2, screenHeight / 2);
	}
	else
	{
		player = new Player((screenWidth - arenaWidth)/2 + SPAWNING_POS, (screenHeight - arenaHeight) / 2 + SPAWNING_POS);
	}
	
	//mouse function
	glutMouseFunc(on_mouse_button);

	//key function 
	glutKeyboardFunc(on_key_press);
	glutKeyboardUpFunc(on_key_release);

	glutIdleFunc(idle);
	glutDisplayFunc(display);

	glutMainLoop();

	return EXIT_SUCCESS;
}

