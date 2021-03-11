//#include <stdlib.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <math.h>
//#include <time.h> 
//#if _WIN32
//#   include <Windows.h>
//#endif
//#if __APPLE__
//#   include <OpenGL/gl.h>
//#   include <OpenGL/glu.h>
//#   include <GLUT/glut.h>
//#else
//#   include <GL/gl.h>
//#   include <GL/glu.h>
//#   include <GL/glut.h>
//#endif
//
//const int initWidth = 600;
//const int initHeight = 600;
//float currentWidth = 600;
//float currentHeight = 600;
//
//float moveX = 0.0;
//float moveY = 0.0;
//float squareSize = 0.2f;
//float bulletSize = 0.2/3;
//float bulletSpeed = 0.0;
//bool isShooting = false;
//bool isAlive = false;
//int enemyRandomPos = 0;
//int squarePosX = 0;
//int squarePosY = 0;
//int bulletPosX = 0;
//int bulletPosY = 0;
//
//void drawGrid()
//{
//	glColor3f(0, 0, 0);
//	for (float x = -1.0; x <= 1.0; x += squareSize)
//	{
//		glBegin(GL_LINES);
//		glVertex3f(x, -1.0, 0);
//		glVertex3f(x, 1.0, 0);
//		glEnd();
//	}
//	for (float y = -1.0; y <= 1.0; y += squareSize)
//	{
//		glBegin(GL_LINES);
//		glVertex3f(-1.0, y, 0);
//		glVertex3f(1, y, 0);
//		glEnd();
//	}
//}
//void drawSquare(float x, float y)
//{
//	glColor3f(0, 0, 0);
//	glBegin(GL_POLYGON);
//	glVertex3f(x , y, 0.0);
//	glVertex3f(x + squareSize, y, 0.0);
//	glVertex3f(x + squareSize, y - squareSize, 0.0);
//	glVertex3f(x , y - squareSize, 0.0);
//	glEnd();
//}
//void drawEnemy(float x, float y)
//{
//	glColor3f(1.0, 0, 1.0);
//	glBegin(GL_POLYGON);
//	glVertex3f(x, y, 0.0);
//	glVertex3f(x + squareSize, y, 0.0);
//	glVertex3f(x + squareSize, y - squareSize, 0.0);
//	glVertex3f(x, y - squareSize, 0.0);
//	glEnd();
//	isAlive = true;
//}
//void killEnemy(float x, float y)
//{
//	glColor3f(1.0, 1.0, 1.0);
//	glBegin(GL_POLYGON);
//	glVertex3f(x, y, 0.0);
//	glVertex3f(x + squareSize, y, 0.0);
//	glVertex3f(x + squareSize, y - squareSize, 0.0);
//	glVertex3f(x, y - squareSize, 0.0);
//	glEnd();
//	isAlive = false;
//}
//void shoot()
//{
//	glColor3f(1.0, 0, 0);
//	glPointSize(5.0);
//	float bulletX = bulletPosX * squareSize + bulletSize;
//	float bulletY = bulletPosY * squareSize + bulletSpeed;
//	glBegin(GL_POLYGON);
//	glVertex3f(bulletX, bulletY, 0.0);
//	glVertex3f(bulletX + bulletSize, bulletY, 0.0);
//	glVertex3f(bulletX + bulletSize, bulletY - bulletSize, 0.0);
//	glVertex3f(bulletX, bulletY - bulletSize, 0.0);
//	glEnd();
//	if (bulletY > 1.0)
//	{
//		isShooting = false;
//		bulletSpeed = 0.0;
//	}
//}
//bool isShot()
//{
//	bool isShot = false;
//	if (isShooting == true)
//	{
//		if (bulletPosY * squareSize + bulletSpeed >= 0.8f)
//		{
//			if (bulletPosX == enemyRandomPos)
//			{
//				isShot = true;
//				printf("is shot");
//			}
//		}
//	}
//	return isShot;
//}
//void display(void)
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(1.0, 1.0, 1.0, 1.0);
//	glEnable(GL_DEPTH_TEST);
//	
//	drawGrid();
//	drawSquare(squarePosX * squareSize, squarePosY * squareSize);
//	if (isAlive == false)
//	{
//		enemyRandomPos = rand() % 10 - 5;
//	}
//	drawEnemy(enemyRandomPos * squareSize, 1.0);
//	
//	if (isShooting == true) 
//	{
//		shoot();
//	}
//	if (isShot() == true)
//	{
//		killEnemy(enemyRandomPos * squareSize, 1.0);
//	}
//
//	//Circle
//	/*glColor3f(1, 1, 0);
//	double r = 0.5;
//	for (double x = -r ; x <= r; x += 0.000001)
//	{
//		glBegin(GL_LINES);
//		double y = sqrt(r*r - x*x);
//		glVertex3f(0, 0, 0);
//		glVertex3f(x, y, 0);
//		glVertex3f(0, 0, 0);
//		glVertex3f(x, -y, 0);
//		glEnd();
//	}*/
//
//	/* Always check for errors! */
//	int err;
//	while ((err = glGetError()) != GL_NO_ERROR)
//		printf("display: %s\n", gluErrorString(err));
//
//	glutSwapBuffers();
//}
//
///* You can ignore this for now, it just lets you exit when you press 'q' or ESC */
//void keyboard(unsigned char key, int x, int y)
//{
//	switch (key)
//	{
//	case 27:
//	case 'q':
//		exit(EXIT_SUCCESS);
//		break;
//	case 'a':
//		if (squarePosX != -5)
//		{
//			squarePosX --;
//		} 
//		break;
//	case 'd':
//		if (squarePosX != 4)
//		{
//			squarePosX++;
//		}
//		break;
//	case 'w':
//		if (squarePosY != 5)
//		{
//			squarePosY++;
//		}
//		break;
//	case 's':
//		if (squarePosY != -4)
//		{
//			squarePosY--;
//		}
//		break;
//	case 'f':
//		if (isShooting == false)
//		{
//			isShooting = true;
//			bulletPosX = squarePosX;
//			bulletPosY = squarePosY;
//			bulletSpeed += bulletSize;
//		}
//		break;
//	default:
//		break;
//	}
//}
//
//void reshape(int width, int height)
//{
//	printf("width = %d\n", width);
//	printf("height = %d\n", height);
//	currentHeight = (float)height;
//	currentWidth = (float)width;
//	float newX, newY;
//	if (currentWidth > currentHeight)
//	{
//		newX = currentWidth / currentHeight;
//		newY = 1.0f;
//	}
//	else
//	{
//		newX = 1.0f;
//		newY = currentHeight / currentWidth;
//	}
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(-newX, newX, -newY, newY, -1.0, 1.0);
//	glViewport(0, 0, currentWidth, currentHeight);
//	glMatrixMode(GL_MODELVIEW);
//	glutPostRedisplay();
//}
//
//void idle()
//{
//	if (bulletSpeed != 0.0) bulletSpeed += 0.05f;
//	glutPostRedisplay();
//}
//
//void init()
//{
//	/* In this program these OpenGL calls only need to be done once,
//	  but normally they would go elsewhere, e.g. display */
//
//	glMatrixMode(GL_PROJECTION);
//	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
//	glMatrixMode(GL_MODELVIEW);
//}
//
//int main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
//	glutCreateWindow("Resize and Move Object");
//	glutReshapeWindow(initWidth, initHeight);
//	init();
//	glutDisplayFunc(display);
//	glutKeyboardFunc(keyboard);
//	glutReshapeFunc(reshape);
//	glutIdleFunc(idle);
//
//	glutMainLoop();
//	return EXIT_SUCCESS;
//}
