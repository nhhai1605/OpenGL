#include <math.h>
#include <time.h> 
#include <iostream>
#include "Player.h"
#if _WIN32
#   include <Windows.h>
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

const int initWidth = 600;
const int initHeight = 600;
float currentWidth = 600;
float currentHeight = 600;
Player * player;
float bulletSize = 0.2 / 3;
float bulletSpeed = 0.0;
bool isShooting = false;
bool isAlive = false;
int enemyPosX = 0;
int enemyPosY = 0;

int bulletPosX = 0;
int bulletPosY = 0;
enum class GunDirection
{
	N, E, S, W, NONE
};

GunDirection gunDir = GunDirection::N;
GunDirection lastGunDir = GunDirection::NONE;

void drawGrid()
{
	glColor3f(0.5, 0.5, 0.5);
	for (float x = -1.0; x <= 1.0; x += SQUARE_SIZE)
	{
		glBegin(GL_LINES);
		glVertex3f(x, -1.0, 0);
		glVertex3f(x, 1.0, 0);
		glEnd();
	}
	for (float y = -1.0; y <= 1.0; y += SQUARE_SIZE)
	{
		glBegin(GL_LINES);
		glVertex3f(-1.0, y, 0);
		glVertex3f(1, y, 0);
		glEnd();
	}
}
void drawSquare(float x, float y)
{
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y - SQUARE_SIZE, 0.0);
	glVertex3f(x, y - SQUARE_SIZE, 0.0);
	glEnd();
}
void drawEnemy(float x, float y)
{
	glColor3f(1.0, 0, 1.0);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y - SQUARE_SIZE, 0.0);
	glVertex3f(x, y - SQUARE_SIZE, 0.0);
	glEnd();
	isAlive = true;
}
void killEnemy(float x, float y)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y, 0.0);
	glVertex3f(x + SQUARE_SIZE, y - SQUARE_SIZE, 0.0);
	glVertex3f(x, y - SQUARE_SIZE, 0.0);
	glEnd();
	isAlive = false;
}
void shoot()
{
	glColor3f(1.0, 0, 0);
	glPointSize(5.0);
	float bulletX = 0;
	float bulletY = 0;
	if (gunDir == GunDirection::N)
	{
		bulletX = bulletPosX * SQUARE_SIZE + bulletSize;
		bulletY = bulletPosY * SQUARE_SIZE + bulletSpeed;
	}
	else if (gunDir == GunDirection::S)
	{
		bulletX = bulletPosX * SQUARE_SIZE + bulletSize;
		bulletY = (bulletPosY - 1) * SQUARE_SIZE + bulletSize - bulletSpeed;
	}
	else if (gunDir == GunDirection::E)
	{
		bulletX = (bulletPosX + 1) * SQUARE_SIZE - bulletSize + bulletSpeed;
		bulletY = bulletPosY * SQUARE_SIZE - bulletSize;
	}
	else if (gunDir == GunDirection::W)
	{
		bulletX = bulletPosX * SQUARE_SIZE - bulletSpeed;
		bulletY = bulletPosY * SQUARE_SIZE - bulletSize;
	}
	glBegin(GL_POLYGON);
	glVertex3f(bulletX, bulletY, 0.0);
	glVertex3f(bulletX + bulletSize, bulletY, 0.0);
	glVertex3f(bulletX + bulletSize, bulletY - bulletSize, 0.0);
	glVertex3f(bulletX, bulletY - bulletSize, 0.0);
	glEnd();
	if (bulletY > 1.0 || bulletY < -1.0 || bulletX > 1.0 || bulletX < -1.0)
	{
		isShooting = false;
		bulletSpeed = 0.0;
	}
}
bool isShot()
{
	bool isShot = false;
	if (isShooting == true)
	{
		if (gunDir == GunDirection::N)
		{
			if (bulletPosY * SQUARE_SIZE + bulletSpeed >= (enemyPosY - 1) * SQUARE_SIZE)
			{
				if (bulletPosX == enemyPosX)
				{
					isShot = true;
					isShooting = false;
					bulletSpeed = 0.0;
				}
			}
		}
		else if (gunDir == GunDirection::S)
		{
			if (bulletPosY * SQUARE_SIZE - bulletSpeed <= (enemyPosY + 1)* SQUARE_SIZE)
			{
				if (bulletPosX == enemyPosX)
				{
					isShot = true;
					isShooting = false;
					bulletSpeed = 0.0;
				}
			}
		}
		else if (gunDir == GunDirection::E)
		{
			if (bulletPosX * SQUARE_SIZE + bulletSpeed >= (enemyPosX - 1) * SQUARE_SIZE)
			{
				if (bulletPosY == enemyPosY)
				{
					isShot = true;
					isShooting = false;
					bulletSpeed = 0.0;
				}
			}			
		}
		else if (gunDir == GunDirection::W)
		{
			if (bulletPosX * SQUARE_SIZE - bulletSpeed <= (enemyPosX + 1) * SQUARE_SIZE)
			{
				if (bulletPosY == enemyPosY)
				{
					isShot = true;
					isShooting = false;
					bulletSpeed = 0.0;
				}
			}
		}
	}
	return isShot;
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();
	drawGrid();
	drawSquare(player->playerPosX * SQUARE_SIZE, player->playerPosY * SQUARE_SIZE);
	if (isAlive == false)
	{
		enemyPosX = rand() % 10 - 5;
		enemyPosY = rand() % 10 - 4;
	}
	drawEnemy(enemyPosX * SQUARE_SIZE, enemyPosY * SQUARE_SIZE);

	if (isShooting == true)
	{
		shoot();
	}
	if (isShot() == true)
	{
		killEnemy(enemyPosX * SQUARE_SIZE, enemyPosY * SQUARE_SIZE);
	}

	/* Always check for errors! */
	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
		printf("display: %s\n", gluErrorString(err));

	glutSwapBuffers();
}

/* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	case 'a':
		if (player->playerPosX != -5)
		{
			if (isShooting == false)
			{
				gunDir = GunDirection::W;
				lastGunDir = GunDirection::NONE;
			}
			else
			{
				lastGunDir = GunDirection::W;
			}
			if(player->playerPosX == enemyPosX + 1)
			{
				if(player->playerPosY != enemyPosY)
				{
					player->playerPosY--;
				}
			}
			else 
			{
				player->playerPosX--;
			}
		}
		break;
	case 'd':
		if (player->playerPosX != 4)
		{
			if (isShooting == false)
			{
				gunDir = GunDirection::E;
				lastGunDir = GunDirection::NONE;
			}
			else
			{
				lastGunDir = GunDirection::E;
			}
			if(player->playerPosX == enemyPosX - 1)
			{
				if(player->playerPosY != enemyPosY)
				{
					player->playerPosX++;
				}
			}
			else 
			{
				player->playerPosX++;
			}
		}
		break;
	case 'w':
		if (player->playerPosY != 5)
		{
			if (isShooting == false)
			{
				gunDir = GunDirection::N;
				lastGunDir = GunDirection::NONE;
			}
			else
			{
				lastGunDir = GunDirection::N;
			}
			if(player->playerPosY == enemyPosY - 1)
			{
				if(player->playerPosX != enemyPosX)
				{
					player->playerPosY++;
				}
			}
			else 
			{
				player->playerPosY++;
			}
		}
		break;
	case 's':
		if (player->playerPosY != -4)
		{
			if (isShooting == false)
			{
				gunDir = GunDirection::S;
				lastGunDir = GunDirection::NONE;
			}
			else
			{
				lastGunDir = GunDirection::S;
			}
			if(player->playerPosY == enemyPosY + 1)
			{
				if(player->playerPosX != enemyPosX)
				{
					player->playerPosY--;
				}
			}
			else 
			{
				player->playerPosY--;
			}
		}
		break;
	case 'f':
		if (isShooting == false)
		{
			isShooting = true;
			bulletSpeed = 0.05f;
			bulletPosX = player->playerPosX;
			bulletPosY = player->playerPosY;
		}
		break;
	default:
		break;
	}
}

void reshape(int width, int height)
{
	currentHeight = (float)height;
	currentWidth = (float)width;
	float newX, newY;
	if (currentWidth > currentHeight)
	{
		newX = currentWidth / currentHeight;
		newY = 1.0f;
	}
	else
	{
		newX = 1.0f;
		newY = currentHeight / currentWidth;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-newX, newX, -newY, newY, -1.0, 1.0);
	glViewport(0, 0, (GLsizei)currentWidth, (GLsizei)currentHeight);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void idle()
{
	if (bulletSpeed != 0.0) 
	{
		bulletSpeed += 0.05f;
	}
	if (lastGunDir != GunDirection::NONE && isShooting == false)
	{
		gunDir = lastGunDir;
	}
	glutPostRedisplay();
}

void init()
{
	/* In this program these OpenGL calls only need to be done once,
	  but normally they would go elsewhere, e.g. display */

	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tank Game");
	glutReshapeWindow(initWidth, initHeight);
	init();
	player = new Player(0, 0);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return EXIT_SUCCESS;
}
