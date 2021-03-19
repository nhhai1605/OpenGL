#include "Player.h"
#include "Bullet.h"
#include "Utility.h"
#include "Asteroid.h"
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

//change this to change to the window(resizable) or fullscreen mode
#define FULLSCREEN_MODE true

//true for polygon ship and false for line ship(like image in the assignment pdf)
#define SHIP_POLYGON true

//true for seeing the objects' hitbox
#define SHOW_HITBOX true

//false to activate the "touch wall then die" mode
#define GO_THROUGH_WALL false


const int initWidth = 600, initHeight = 600;
float currentWidth = 0.0f, currentHeight = 0.0f, lastWidth = 0.0f, lastHeight = 0.0f;
float bulletSpeed = INIT_BULLET_SPEED;
float bulletSize = INIT_BULLET_SIZE;
float currentFPS = 0.0f, lastFPS = 0.0f;
Player * player;
std::vector <Bullet*> bullets;
std::vector <Asteroid*> asteroids;
static float last_t = 0.0f, dt = 0.0f;
float shoot_delay = 0.0f, fps_update_delay = 0.0f, asteroid_spawn_delay = 0.0f;
bool shootable = true;
bool isThrusting = false;
bool FPSupdatable = true;
enum class KeyState {FREE, PRESSED} keyArray[127], mousePressed;

float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2)*(y1 - y2));
}

void drawPlayerInfo(Player * player)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, currentWidth, 0 , currentHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(50, currentHeight - 50);
	std::string live = "Live: " + std::to_string(player->live);
	for (std::string::iterator i = live.begin(); i != live.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glRasterPos2f(50, currentHeight - 100);
	std::string score = "Score: " + std::to_string(player->score);
	for (std::string::iterator i = score.begin(); i != score.end(); ++i)
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
	float x1, x2, x3, y1, y2, y3;
	//these cordinates are calculated with rotation formula: 
	//x' = x * cos(theta) - y * sin(theta)
	//y' = x * sin(theta) + y * cos(theta)
	//with x and y is 0 and theta is playerDirectionRadian
	//when player move, we count new playerX and playerY is 0, so x and y is 0
	//we just need to add playerX and playerY after calculate
	x1 = size * cos(radian) + size * sin(radian);
	y1 = size * sin(radian) - size * cos(radian);
	x2 = -size * 1.5 * sin(radian);
	y2 = size * 1.5 * cos(radian);
	x3 = -size * cos(radian) + size * sin(radian);
	y3 = -size * sin(radian) - size * cos(radian);
	
	//Draw the ship	
	glColor3f(1.0, 1.0, 1.0);
	if (SHIP_POLYGON == true)
	{
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(x1 + x, y1 + y, 0.0);
		glVertex3f(x2 + x, y2 + y, 0.0);
		glVertex3f(x3 + x, y3 + y, 0.0);
		glEnd();
	}
	else
	{
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

	if (isThrusting == true)
	{
		//the cordinates of the rocket boost
		float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4, rx5, ry5, rx6, ry6;
		rx1 = size / 2.5 * cos(radian) + size / 2.5 * sin(radian);
		ry1 = size / 2.5 * sin(radian) - size / 2.5 * cos(radian);
		rx2 = size * sin(radian);
		ry2 = -size * cos(radian);
		rx3 = -size / 2.5 * cos(radian) + size / 2.5 * sin(radian);
		ry3 = -size / 2.5 * sin(radian) - size / 2.5 * cos(radian);

		rx4 = size / 4 * cos(radian) + size / 4 * sin(radian);
		ry4 = size / 4 * sin(radian) - size / 4 * cos(radian);
		rx5 = size * 0.75 * sin(radian);
		ry5 = -size * 0.75 * cos(radian);
		rx6 = -size / 4 * cos(radian) + size / 4 * sin(radian);
		ry6 = -size / 4 * sin(radian) - size / 4 * cos(radian);

		//Draw the rocket boost
		glColor3f(1.0, 0.5, 0.05);
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(rx1 + x, ry1 + y, 0.5);
		glVertex3f(rx2 + x, ry2 + y, 0.0);
		glVertex3f(rx3 + x, ry3 + y, 0.0);
		glEnd();

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0.0);
		glVertex3f(rx4 + x, ry4 + y, 0.5);
		glVertex3f(rx5 + x, ry5 + y, 0.5);
		glVertex3f(rx6 + x, ry6 + y, 0.5);
		glEnd();
	}
}

void drawBullet(Bullet * bullet)
{
	glPointSize(bulletSize);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	glVertex3f(bullet->posX, bullet->posY, 0.0);
	glEnd();
}

void drawAsteroid(Asteroid* asteroid)
{
	glColor3f(0.5, 0.5, 0.5);
	float angle = 360 / asteroid->vertices;
	glBegin(GL_POLYGON);
	float radian = angle * PI / 180;
	for (int i = 0; i < asteroid->vertices; i++)
	{
		float x = ASTEROID_RADIUS * cos(radian * i) + ASTEROID_RADIUS * sin(radian * i);
		float y = ASTEROID_RADIUS * sin(radian * i) - ASTEROID_RADIUS * cos(radian * i);
		x *= asteroid->offsets[i] / 10;
		y *= asteroid->offsets[i] / 10;
		glVertex3f(x + asteroid->posX, y + asteroid->posY, 0.0);
	}
	glEnd();
	if (SHOW_HITBOX == true)
	{
		float r = ASTEROID_RADIUS;
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

void drawFPS(std::string fps)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, currentWidth, 0 , currentHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(0, 0);
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

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	drawFPS(std::to_string(static_cast<int>(lastFPS)));
	drawPlayerInfo(player);
	drawPlayer(player);
	for (auto& bullet : bullets)
	{
		drawBullet(bullet);
	}
	for (auto& asteroid : asteroids)
	{
		drawAsteroid(asteroid);
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

void reshape(int width, int height)
{
	currentHeight = (float)height;
	currentWidth = (float)width;
	if (lastWidth == 0.0f)
	{
		lastWidth = currentWidth;
	}
	if (lastHeight == 0.0f)
	{
		lastHeight = currentWidth;
	}


	player->playerSize = INIT_PLAYER_SIZE * (currentHeight + currentWidth) / (initHeight + initWidth);
	player->thrustSpeed = player->playerSize / 2;
	bulletSize = INIT_BULLET_SIZE * (currentHeight + currentWidth) / (initHeight + initWidth);
	bulletSpeed = bulletSize / INIT_BULLET_SIZE * INIT_BULLET_SPEED;
	
	player->posX = player->posX / lastWidth * currentWidth ;
	player->posY = player->posY / lastHeight * currentHeight;
	for (auto& bullet : bullets)
	{
		bullet->posX = bullet->posX / lastWidth * currentWidth;
		bullet->posY = bullet->posY / lastHeight * currentHeight;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-currentWidth / 2, currentWidth / 2, -currentHeight / 2, currentHeight / 2, -1.0, 1.0);
	glViewport(0, 0, currentWidth, currentHeight);
	glMatrixMode(GL_MODELVIEW);
	lastWidth = currentWidth;
	lastHeight = currentHeight;
	glutPostRedisplay();
}

void on_mouse_button(int btn, int state, int x, int y) 
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

void update_game_state(float dt)
{
	//Asteroid spaw delay time
	asteroid_spawn_delay += dt;
	if (asteroid_spawn_delay >= 2000.0 && asteroids.size() < INIT_ASTEROIDS_MAX)
	{
		int int_currentWidth = static_cast<int>(currentWidth);
		int int_currentHeight = static_cast<int>(currentHeight);
		float asteroidX = rand() % 100 + (int_currentWidth / 2); //100 just to make sure the asteroid is spawning outside the wall 
		float asteroidY = rand() % 100 + (int_currentHeight / 2);
		Asteroid* asteroid = new Asteroid(asteroidX, asteroidY);
		asteroids.push_back(asteroid);
		asteroid_spawn_delay = 0.0;
	}

	//Shoot delay time
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
		Bullet* bullet = new Bullet(posX, posY, radian);
		bullets.push_back(bullet);
	}

	//FPS update delay time
	currentFPS = 1000 / dt;
	fps_update_delay += dt;
	if (fps_update_delay >= FPS_UPDATE_DELAY) //2 seconds
	{
		fps_update_delay = 0.0f;
		lastFPS = currentFPS;
	}
	//delete bullet that are out of the wall
	for (int i = 0; i < bullets.size(); i++)
	{
		auto& bullet = bullets[i];
		if (bullet->posX > currentWidth / 2 + BULLET_MAX_DIS_BEHIND_WALL || bullet->posX < -currentWidth / 2 - BULLET_MAX_DIS_BEHIND_WALL
			|| bullet->posY > currentHeight / 2 + BULLET_MAX_DIS_BEHIND_WALL || bullet->posY < -currentHeight / 2 - BULLET_MAX_DIS_BEHIND_WALL)
		{
			bullets.erase(bullets.begin() + i);
			break;
		}
	}
	for (auto& bullet : bullets)
	{
		bullet->posX -= sin(bullet->bulletDirectionRadian) * bulletSpeed / currentFPS;
		bullet->posY += cos(bullet->bulletDirectionRadian) * bulletSpeed / currentFPS;
	}
	for (auto& asteroid : asteroids)
	{
		if (asteroid->posX > currentWidth / 2 + ASTEROID_RADIUS)
		{
			asteroid->posX = -currentWidth / 2 - ASTEROID_RADIUS;
		}
		else if (asteroid->posX < -currentWidth / 2 - ASTEROID_RADIUS)
		{
			asteroid->posX = currentWidth / 2 + ASTEROID_RADIUS;
		}
		if (asteroid->posY > currentHeight / 2 + ASTEROID_RADIUS)
		{
			asteroid->posY = -currentHeight / 2 - ASTEROID_RADIUS;
		}
		else if (asteroid->posY < -currentHeight / 2 - ASTEROID_RADIUS)
		{
			asteroid->posY = currentHeight / 2 + ASTEROID_RADIUS;
		}
	}
	for (auto& asteroid : asteroids)
	{
		asteroid->posX -= sin(asteroid->asteroidDirectionRadian) * asteroid->asteroidSpeed / currentFPS;
		asteroid->posY += cos(asteroid->asteroidDirectionRadian) * asteroid->asteroidSpeed / currentFPS;
	}


	//Ship movement
	if (keyArray[int('a')] == KeyState::PRESSED)
	{
		player->rotateLeft(dt);
	}
	if (keyArray[int('d')] == KeyState::PRESSED)
	{
		player->rotateRight(dt);
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
	}
	else
	{
		isThrusting = false;
		player->velocityX -= FRICTION * player->velocityX / currentFPS;
		player->velocityY -= FRICTION * player->velocityY / currentFPS;
	}
	player->move();

	if(GO_THROUGH_WALL == true)
	{
		//ship go through wall situation
		if (player->posX < (-currentWidth / 2 - player->playerSize) && player->velocityX < 0)
		{
			player->posX = currentWidth / 2 + player->playerSize * 2;
		}
		else if (player->posX > (currentWidth / 2 + player->playerSize) && player->velocityX > 0)
		{
			player->posX = -currentWidth / 2 - player->playerSize * 2;
		}
	
		if (player->posY < (-currentHeight / 2 - player->playerSize) && player->velocityY < 0)
		{
			player->posY = currentHeight / 2 + player->playerSize * 2;
		}
		else if (player->posY > (currentHeight / 2 + player->playerSize) && player->velocityY > 0)
		{
			player->posY = -currentHeight / 2 - player->playerSize * 2;
		}
	}
	else 
	{
		if (player->posX < (-currentWidth / 2 + player->playerSize))
		{
			player->die();
		}
		else if(player->posX > (currentWidth / 2 - player->playerSize))
		{
			player->die();
		}
		if (player->posY < (-currentHeight / 2 + player->playerSize))
		{
			player->die();
		}
		else if(player->posY > (currentHeight / 2 - player->playerSize))
		{
			player->die();
		}
	}
	for (int i = 0; i < bullets.size(); i++)
	{
		for (int j = 0; j < asteroids.size(); j++)
		{
			if(distance(bullets[i]->posX, bullets[i]->posY, asteroids[j]->posX, asteroids[j]->posY) <= (ASTEROID_RADIUS + bulletSize))
			{
				player->score += 100;
				bullets.erase(bullets.begin() + i);
				asteroids.erase(asteroids.begin() + j);
			}
		}
	}
	for (int i = 0; i < asteroids.size(); i++)
	{
		if(distance(player->posX, player->posY, asteroids[i]->posX, asteroids[i]->posY) <= (ASTEROID_RADIUS + player->playerSize))
			{
				asteroids.erase(asteroids.begin() + i);
				player->die();
				if(player->live == 0)
				{
					exit(EXIT_SUCCESS);
				}
			}
	}

}
void idle()
{
	float t = glutGet(GLUT_ELAPSED_TIME);
	dt = t - last_t;
	update_game_state(dt);
	last_t = t;
	glutPostRedisplay();
}

void init(int initScreenWidth, int initScreenHeight)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(-initScreenWidth / 2, initScreenWidth / 2, -initScreenHeight / 2, initScreenHeight / 2, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	

	if (FULLSCREEN_MODE == false)
	{
		glutInitWindowPosition((screenWidth - initWidth) / 2, (screenHeight - initHeight) / 2);
		glutInitWindowSize(initWidth, initHeight);
		init(initWidth, initHeight);
	}
	glutCreateWindow("Assignment 1");
	if (FULLSCREEN_MODE == true)
	{
		init(screenWidth, screenHeight);
		glutFullScreen();
	}
	srand(time(NULL));
	player = new Player(0, 0);

	glutReshapeFunc(reshape);

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

