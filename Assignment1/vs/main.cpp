#include <math.h>
#include <iostream>
#include "Player.h"
#include "Bullet.h"
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

#define FPS 60

const int initWidth = 600, initHeight = 600;
float currentWidth = 0.0f, currentHeight = 0.0f, lastWidth = 0.0f, lastHeight = 0.0f;
float currentWindowX = 0.0f, currentWindowY = 0.0f;
Player * player;
Bullet * bullet;
int bulletSize = 10;
static float last_t = 0, dt = 0;
enum KeyState {FREE, PRESSED} keyArray[127];

void drawPoint(float currentWindowX, float currentWindowY)
{
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glVertex3f(currentWindowX, currentWindowY, 0.0);
	glEnd();
}

void drawPlayer(float currentWindowX, float currentWindowY)
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
	x2 = -size * 2 * sin(radian);
	y2 = size * 2 * cos(radian);
	x3 = -size * cos(radian) + size * sin(radian);
	y3 = -size * sin(radian) - size * cos(radian);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x1 + x, y1 + y, 0.0);
	glVertex3f(x2 + x, y2 + y, 0.0);
	glVertex3f(x3 + x, y3 + y, 0.0);
	glEnd();
}

void drawBullet(Bullet * bullet)
{
	glPointSize((float)bulletSize);
	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();
	glRotatef(bullet->bulletDirectionRadian, 0, 0, 1.0);
	glBegin(GL_POINTS);
	glVertex3f(bullet->bulletX, bullet->bulletY, 0.0);
	glEnd();
	glPopMatrix();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	drawBullet(bullet);
	drawPlayer(currentWindowX, currentWindowY);
	drawPoint(currentWindowX, currentWindowY);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glEnd();

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
		exit(EXIT_SUCCESS);
	if (key == 'w')
	{
		keyArray[int('w')] = PRESSED;
	}
	if (key == 'a')
	{
		keyArray[int('a')] = PRESSED;
	}
	if (key == 'd')
	{
		keyArray[int('d')] = PRESSED;
	}
	glutPostRedisplay();
}

void on_key_release(unsigned char key, int x, int y)
{
	if (key == 'w')
	{
		keyArray[int('w')] = FREE;
	}
	if (key == 'a')
	{
		keyArray[int('a')] = FREE;
	}
	if (key == 'd')
	{
		keyArray[int('d')] = FREE;
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

	player->posX = player->posX / lastWidth * currentWidth ;
	player->posY = player->posY / lastHeight * currentHeight;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-currentWidth / 2, currentWidth / 2, -currentHeight / 2, currentHeight / 2, -1.0, 1.0);
	glViewport(0, 0, currentWidth, currentHeight);
	glMatrixMode(GL_MODELVIEW);
	lastWidth = currentWidth;
	lastHeight = currentHeight;
	glutPostRedisplay();
}

void init(int initScreenWidth, int initScreenHeight)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(-initScreenWidth /2, initScreenWidth /2, -initScreenHeight /2, initScreenHeight /2, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
}

void on_mouse_move(int mouseX, int mouseY)
{
	currentWindowX = mouseX - currentWidth / 2;
	currentWindowY = -mouseY + currentHeight / 2;
}

void CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;
	static int fps;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount64() * 0.001f;
	++framesPerSecond;
	//std::cout << "FPS: " << fps << std::endl;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		fps = (int)framesPerSecond;
		framesPerSecond = 0;
	}
}

void idle()
{
	CalculateFrameRate();
	float t = glutGet(GLUT_ELAPSED_TIME);
	dt = t - last_t;
	if (keyArray[int('a')] == PRESSED)
	{
		player->rotateLeft(dt);
	}
	if (keyArray[int('d')] == PRESSED)
	{
		player->rotateRight(dt);
	}
	if (keyArray[int('w')] == PRESSED)
	{
		if (player->velocityX <= MAX_SPEED)
		{
			player->velocityX -= THRUST_SPEED * sin(player->playerDirectionRadian) / FPS;
		}
		if (player->velocityY <= MAX_SPEED)
		{
			player->velocityY += THRUST_SPEED * cos(player->playerDirectionRadian) / FPS;
		}
	}
	else
	{
		player->velocityX -= FRICTION * player->velocityX / FPS;
		player->velocityY -= FRICTION * player->velocityY / FPS;
	}
	player->move(dt);

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
		player->posY = currentHeight / 2 + player->playerSize * 2 ;
	}
	else if (player->posY > (currentHeight / 2 + player->playerSize) && player->velocityY > 0)
	{
		player->posY = -currentHeight / 2 - player->playerSize * 2;
	}
	last_t = t;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition((screenWidth - initWidth) / 2, (screenHeight - initHeight) / 2);
	glutInitWindowSize(initWidth, initHeight);
	glutCreateWindow("Assignment 1");
	init(initWidth, initHeight);

	player = new Player(0, 0);
	bullet = new Bullet(0, 0, player->playerDirectionRadian);
	//	glutFullScreen();

	glutPassiveMotionFunc(on_mouse_move);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(on_key_press);
	glutKeyboardUpFunc(on_key_release);
	glutDisplayFunc(display);
	glutMainLoop();

	return EXIT_SUCCESS;
}

