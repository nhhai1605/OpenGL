//this file is used to contain all the variable that can be adjusted
//some variables that are required to be adjustable in the specs is in the main.cpp
//so that you can change it easier
#pragma once

#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
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

#define KEY_ESC 27
#define PLAYER_SIZE 50.0f
#define THRUST_SPEED 50.0f
#define MAX_SPEED 80.0f
#define ROTATION_SPEED 2.5f
#define PI 3.14159265f
#define FRICTION 0.9f
#define ASTEROID_FRICTION 0.01f
#define ASTEROID_SIZE 1500
#define TIME_DELAY_BETWEEN_TWO_SHOTS 50.0f 
#define BULLET_SIZE 25.0f
#define BULLET_SPEED 50000.0f
#define FPS_UPDATE_DELAY 2000.0f //2 seconds
#define ASTEROID_SPEED 5000
#define ASTEROID_MAX_VELOCITY 100.0f
#define WARNING_DISTANCE 10000.0f
#define INIT_ASTEROIDS_NUMBER 3
#define LIFE 3
#define INVULNERABLE_TIME 2000
#define INIT_PARTICLE_SIZE 10.0f
#define BLINKING_DUR 100
#define PLAYER_DAMGE 200
#define EXPLOSION_DECAY_TIME 1000.0f
#define EXPLOSION_SPEED 2000.0f
#define EXPLOSION_PARTICLE_SIZE 5.0f
#define BLACK_HOLE_RADIUS 150.0f
#define BL_MASS 1000.0f
#define NORM_DISTANCE 1000.0f
#define SUN_SIZE 5000.0f
enum class WallWarning { TOP, BOTTOM, FRONT, BACK, LEFT, RIGHT};
enum class KeyState { FREE, PRESSED };
enum class AsteroidState { SPLITTABLE, UNSPLITTABLE };
class vector3
{	
public:
	float x, y, z;
	vector3() {}
	vector3(float x, float y, float z)
	{
		this->x = x; this->y = y; this->z = z;
	}
	vector3 operator+(vector3 v) { return vector3(v.x + x, v.y + y, v.z + z); }
	vector3 operator-(vector3 v) { return vector3(x - v.x, y - v.y, z - v.z); }
	vector3 operator*(float n) { return vector3(x * n, y * n, z * n); }
	vector3 operator/(float n) { return vector3(x / n, y / n, z / n); }					
};
float angleToRadian(float angle)
{
	return angle * PI / 180;
}
float radianToAngle(float radian)
{
	return radian * 180 / PI;
}
float dot(vector3 v1, vector3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float magnitude(vector3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
float sqr(float a)
{
	return a * a;
}
vector3 cross(vector3 v1, vector3 v2)
{
	return vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}
vector3 normalize(vector3 v)
{
	float length = magnitude(v);
	return vector3(v.x / length, v.y / length, v.z / length);
}
float* quadratic(float a, float b, float c)
{
	float result[2];
	float delta = sqr(b) - 4 * a * c;
	if (delta < 0)
	{
		return nullptr;
	}
	else if (delta == 0)
	{
		result[0] = -b / a / 2.0f;
		result[1] = -b / a / 2.0f;
	}
	else
	{
		result[0] = (-b + sqrt(delta))/ a / 2.0f;
		result[1] = (-b - sqrt(delta)) / a / 2.0f;
	}
	return result;
}