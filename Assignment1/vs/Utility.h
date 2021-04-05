//this file is used to contain all the variable that can be adjusted
//some variables that are required to be adjustable in the specs is in the main.cpp
//so that you can change it easier
#ifndef UTILITY 
#define UTILITY
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
#define PLAYER_SIZE 25.0f
#define MAX_SPEED 30.0f
#define ROTATION_SPEED 5.0f
#define PI 3.14159265f
#define FRICTION 0.8f
#define ASTEROID_FRICTION 0.01f
#define TIME_DELAY_BETWEEN_TWO_SHOTS 100.0f // 100 milliseconds
#define BULLET_SIZE 10.0f
#define BULLET_SPEED 1000.0f
#define FPS_UPDATE_DELAY 2000.0f //2 seconds
#define ASTEROID_SPEED 250
#define ASTEROID_MAX_VELOCITY 5.0f
#define WARNING_DISTANCE 200.0f
#define INIT_ASTEROIDS_NUMBER 3
#define LIFE 3
#define INVULNERABLE_TIME 2000
#define INIT_PARTICLE_SIZE 10.0f
#define BLINKING_DUR 100
#define PLAYER_DAMGE 10
#define EXPLOSION_DECAY_TIME 1000.0f
#define EXPLOSION_SPEED 200.0f
#define EXPLOSION_PARTICLE_SIZE 7.5f
#define BLACK_HOLE_RADIUS 150.0f
#define BL_MASS 1000.0f
enum class WallWarning { N, E, S, W };
enum class KeyState { FREE, PRESSED };
enum class AsteroidState { SPLITTABLE, UNSPLITTABLE };

#endif // UTILITY