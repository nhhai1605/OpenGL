#ifndef UTILITY
#define UTILITY
#include <math.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
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
#define BLINKING_DUR 100
#define PLAYER_DAMGE 10
enum class WallWarning { N, E, S, W };
enum class KeyState { FREE, PRESSED };
enum class AsteroidState { SPLITED, NOTSPLITED };
#endif // UTILITY