#include <math.h>
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

float currentWidth = 0;
float currentHeight = 0;
Player * player;
void drawPlayer(float playerX, float playerY)
{
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(playerX, playerY, 0.0);
	glVertex3f(playerX + 0.05, playerY - 0.05, 0.0);
	glVertex3f(playerX, playerY + 0.1, 0.0);
	glVertex3f(playerX - 0.05, playerY - 0.05, 0.0);
	glEnd();
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	
	drawPlayer(player->playerX, player->playerY);
	
	/* Always check for errors! */
	int err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "display: " << gluErrorString(err)<< std::endl;
	}
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
	case 'w':
		player->move();
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
	glViewport(0, 0, currentWidth, currentHeight);
	glMatrixMode(GL_MODELVIEW);
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

void on_mouse_move(int x, int y)
{
//	std::cout << x << " " << y << std::endl;
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow("Assignment 1");
	std::cout << screenWidth << std::endl;

	init();
	
	player = new Player(0.0, 0.0);
//	glutFullScreen();
	glutDisplayFunc(display);
	glutPassiveMotionFunc(on_mouse_move);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return EXIT_SUCCESS;
}

