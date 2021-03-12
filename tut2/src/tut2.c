#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	//Q1
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();

	//Q2
	glColor3f(1.0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glEnd();


	//Q3: counter clockwise
	glColor3f(0, 1.0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-0.25, -0.25, -0.75);
	glVertex3f(0.75, -0.25, -0.75);
	glVertex3f(0.75, 0.75, -0.75);
	glVertex3f(-0.25, 0.75, -0.75);
	glEnd();

	float r = 0.5f;
	glColor3f(1.0, 1.0, 1.0);
	for(float x = -r; x <= r; x += 0.000001f)
	{
		glBegin(GL_LINES);
		float y = 0;
		if(x > -r && x < r)
		{
			y = (float)sqrt(r * r - x * x);
		}
		glVertex3f(0, 0, 0);
		glVertex3f(x, y, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(x, -y, 0);
		glEnd();
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Tutorial 1");
	glutReshapeWindow(600, 600);
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return EXIT_SUCCESS;
}
