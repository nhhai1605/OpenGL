#include "Utility.h"
#include "Player.h"
#include "Bullet.h"
#include "Utility.h"
#include "Asteroid.h"
#include "Particle.h"
#include "Explosion.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float arenaSize = 20000.0f;
float arena_color[] = { 0.5, 0.5, 0.5 };
float asteroid_color[] = { 0.5, 0.5, 0.5 };
float player_color[] = { 0.0, 1.0, 0.0 };

//Boost particle
int particleNum = 20000; //max number of particles 
float decayTime = 3000.0f; // decay time of each particle (ms)
float sizeReduceRate = INIT_PARTICLE_SIZE / decayTime; //the reduce rate of particle size in ms
float particle_color[] = { 1.0f, 0.2f, 0.02f, 0.8f }; //particle init color (before any change when decaying)
float particleSpeed = 500.0f; //particle speed when going out from the back of the ship
float timeBetweenParticles = 20.0f; //time delaying before each particle
float particle_dt = 0.0f;

//Explosion
int explosionParticles = 200;
float explosion_color[] = { 1.0f, 0.0f, 0.0f , 0.8f};
//Bullet color
float bullet_color[] = { 1.0, 0.0, 0.0 };

//Wall Warning color
float warning_color[] = { 1.0, 0.0, 0.0 };

//change number of init asteroids here
int currRound = 1; //current round
int asteroids_number = INIT_ASTEROIDS_NUMBER * currRound; //number of asteroids in each rounds
float asteroid_spawn_rate = 2000.0f; //time delay between each spawn (ms)
float scale;
Player* player;
std::vector <Bullet*> bullets;
std::vector <Asteroid*> asteroids;
KeyState keyArray[127], mousePressed[2];
std::vector <WallWarning> warnings;
std::vector <Particle*> particles;
std::vector <Explosion*> explosions;
Camera* camera;
//Global variables
float screenWidth, screenHeight; //screen width and height of user's screen
float last_t = 0.0f, dt = 0.0f; //this is delta time, which are used in many cases
float currentFPS = 60.0f, lastFPS = 0.0f; //these are used for displaying fps
float shoot_delay = 0.0f, fps_update_delay = 0.0f, asteroid_spawn_delay = 0.0f; //these variables are used for delaying between each bullets, each asteroids,..
//These boolean variables are used to keep tracked of player state or game state
bool shootable = true;
bool isThrusting = false;
bool FPSupdatable = true;
bool gameStarted = false;
bool playerDied = false;
float time_since_game_start = 0.0f; // this is used to calculate time in game
//these are used to calculate and displaying the blinking and invulnerable time after you die
float after_die_time = 0.0f;
bool invulnerable_state = false;
int blinking_num = INVULNERABLE_TIME / BLINKING_DUR;
float blinking_time = BLINKING_DUR;
float wingLeft = 0.0f, wingRight = 0.0f;

void restartGame()
{
    //set every thing back at the beginning state and also make a black hole at other random position
    playerDied = false;
    player->respawn();
    asteroids.clear();
    particles.clear();
    explosions.clear();
    currRound = 1;
    int asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
    //blackHoleX = (rand() % (int)(arenaWidth - 2 * BLACK_HOLE_RADIUS)) + (screenWidth - arenaWidth) / 2 + BLACK_HOLE_RADIUS;
    //blackHoleY = (rand() % (int)(arenaHeight - 2 * BLACK_HOLE_RADIUS)) + (screenHeight - arenaHeight) / 2 + BLACK_HOLE_RADIUS;
}

void drawFPS(std::string fps)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(10, 10);
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

void drawArena()
{
    boolean top = false, bottom = false, front = false, back = false, left = false, right = false;
    for (WallWarning warning : warnings)
    {
        if (warning == WallWarning::TOP && gameStarted && !playerDied)
        {
            top = true;
        }
        if (warning == WallWarning::BOTTOM && gameStarted && !playerDied)
        {
            bottom = true;
        }
        if (warning == WallWarning::RIGHT && gameStarted && !playerDied)
        {
            right = true;
        }
        if (warning == WallWarning::LEFT && gameStarted && !playerDied)
        {
            left = true;
        }
        if (warning == WallWarning::BACK && gameStarted && !playerDied)
        {
            back = true;
        }
        if (warning == WallWarning::FRONT && gameStarted && !playerDied)
        {
            front = true;
        }
    }
    glDisable(GL_LIGHTING);
    for (float i = -arenaSize; i <= arenaSize; i += arenaSize/10)
    {
        glBegin(GL_LINES);

        //bottom
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (bottom)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        /*glTexCoord3f(-arenaSize, -arenaSize, i);
        glTexCoord3f(arenaSize, -arenaSize, i);
        glTexCoord3f(i, -arenaSize, -arenaSize);
        glTexCoord3f(i, -arenaSize, arenaSize);*/
        glVertex3f(-arenaSize, -arenaSize, i); //-100,y,-100
        glVertex3f(arenaSize, -arenaSize, i);//100,y,-100
        glVertex3f(i, -arenaSize, -arenaSize);//-100,y,-100
        glVertex3f(i, -arenaSize, arenaSize);//100,y,100

        //top
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (top)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        glVertex3f(-arenaSize, arenaSize, i);
        glVertex3f(arenaSize, arenaSize, i);
        glVertex3f(i, arenaSize, -arenaSize);
        glVertex3f(i, arenaSize, arenaSize);

        //left
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (left)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        glVertex3f(arenaSize, -arenaSize, i);
        glVertex3f(arenaSize, arenaSize, i);
        glVertex3f(arenaSize, i, -arenaSize);
        glVertex3f(arenaSize, i, arenaSize);

        //right
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (right)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        glVertex3f(-arenaSize, -arenaSize, i);
        glVertex3f(-arenaSize, arenaSize, i);
        glVertex3f(-arenaSize, i, -arenaSize);
        glVertex3f(-arenaSize, i, arenaSize);

        //front
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (front)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        glVertex3f(i, -arenaSize, arenaSize);
        glVertex3f(i, arenaSize, arenaSize);
        glVertex3f(-arenaSize, i, arenaSize);
        glVertex3f(arenaSize, i, arenaSize);

        //back
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        if (back)
        {
            glColor3f(warning_color[0], warning_color[1], warning_color[2]);
        }
        glVertex3f(i, -arenaSize, -arenaSize);
        glVertex3f(i, arenaSize, -arenaSize);
        glVertex3f(-arenaSize, i, -arenaSize);
        glVertex3f(arenaSize, i, -arenaSize);
        glColor3f(arena_color[0], arena_color[1], arena_color[2]);
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

void drawCrosshair()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(screenWidth / 2 - 20.0f, screenHeight / 2, 1.0f);
    glVertex3f(screenWidth / 2 + 20.0f, screenHeight / 2, 1.0f);
    glVertex3f(screenWidth / 2, screenHeight /2 - 20.0f, 1.0f);
    glVertex3f(screenWidth / 2, screenHeight / 2 + 20.0f, 1.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera->getZoom(), screenWidth / screenHeight, 0.1, (double)arenaSize * 3.0f);
}

//this fucntion is used to draw the message at the start of the game
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
    glRasterPos2f(screenWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (str.size() / 2 - 2), screenHeight / 2);
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

//this fucntion is used to draw the message when player have no life left
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
    glRasterPos2f(screenWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (str.size() / 2 - 2), screenHeight / 2);
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

///this function is used to draw player's information, like life left, score, number of destroyed asteroids and time has passed in the game
void drawPlayerInfo(Player* player)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);

    std::string live = "Life: " + std::to_string(player->life);
    glRasterPos2f(screenWidth / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (live.size() - 3), screenHeight - 25);
    for (std::string::iterator i = live.begin(); i != live.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    std::string score = "Score: " + std::to_string(player->score); 
        glRasterPos2f(10, screenHeight - 25);
    for (std::string::iterator i = score.begin(); i != score.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    std::string asteroidsDestroyed = "Destroyed: " + std::to_string(player->asteroidsDestroyed);
    glRasterPos2f(screenWidth - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, 'a') * (asteroidsDestroyed.size() + 1), screenHeight - 25);
    for (std::string::iterator i = asteroidsDestroyed.begin(); i != asteroidsDestroyed.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    int t = (int)time_since_game_start;
    int minutes = (int)floor(t / 60000);
    int seconds = (int)floor((t - minutes * 60000) / 1000);
    std::string time = "Time: " + std::to_string(minutes) + ":" + std::to_string(seconds);
    glRasterPos2f(150, 10);
    for (std::string::iterator i = time.begin(); i != time.end(); ++i)
    {
        char c = *i;
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    std::string round = "Round: " + std::to_string(currRound);
    glRasterPos2f(300, 10);
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

void drawPlayer(Player* player)
{
    float mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    float mat_diffuse[] = { 1.0, 0.8, 0.2, 1.0 };
    float die_color[] = { 0.0, 0.0, 0.0, 1.0 };
    float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };          
    float mat_shininess[] = { 100.0 };     
                
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);    
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    float x = player->position.x, y = player->position.y, z = player->position.z;
    float size = player->playerSize;
    if (invulnerable_state)
    {
        //blinking here is used to make player blink when they are in the invulnerable state (after respawning)
        if (blinking_num % 2 == 0)
        {
            //draw the ship
            glPushMatrix();

            glTranslatef(x, y, z);
            glRotatef(-player->horRadian * 180 / PI, 0.0, 1.0, 0.0);
            glRotatef(player->verRadian * 180 / PI, 1.0f, 0.0f, 0.0f);

            glPushMatrix();
            glScalef(0.4, 0.4, 1.25f);
            glutSolidSphere(size, 100, 100);
            glPopMatrix();

            glPushMatrix();
            glTranslated(0.0, 0.0, -15.0);
            glScaled(0.1, 0.1, 1.0);
            glutSolidTorus(size, size, 100, 100);
            glPopMatrix();


            //left
            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            glRotatef(20 - wingLeft, 0.0, 0.0, 1.0);
            glTranslatef(50.0, 5.0, -30.0);
                glPushMatrix();
                glScalef(2.0f, 0.1, 0.6);
                glutSolidCube(size);
                glPopMatrix();
            glTranslatef(55.0, 0.0, 0.0);
            glScaled(0.075, 0.075, 1.0);
            glutSolidTorus(size - 30.0, size, 100, 100);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            glRotatef(-20 + wingRight, 0.0, 0.0, 1.0);
            glTranslatef(50.0, -5.0, -30.0);
                glPushMatrix();
                glScalef(2.0f, 0.1, 0.6);
                glutSolidCube(size);
                glPopMatrix();
            glTranslatef(55.0, 0.0, 0.0);
            glScaled(0.075, 0.075, 1.0);
            glutSolidTorus(size - 30.0, size, 100, 100);
            glPopMatrix();

            ////right
            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            glRotatef(20 - wingLeft, 0.0, 0.0, 1.0);
            glTranslatef(-50.0, -5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
            glTranslatef(-55.0, 0.0, 0.0);
            glScaled(0.075, 0.075, 1.0);
            glutSolidTorus(size - 30.0, size, 100, 100);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            glRotatef(-20 + wingRight, 0.0, 0.0, 1.0);
            glTranslatef(-50.0, 5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
            glTranslatef(-55.0, 0.0, 0.0);
            glScaled(0.075, 0.075, 1.0);
            glutSolidTorus(size - 30.0, size, 100, 100);
            glPopMatrix();

            glPopMatrix();
        }
        //this is used to managed the blinking effect
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
        //this is just the same, but when the player is not in the invulnerable state
        glPushMatrix();

        glTranslatef(x, y, z);
        glRotatef(-player->horRadian * 180 / PI, 0.0, 1.0, 0.0);
        glRotatef(player->verRadian * 180 / PI, 1.0f, 0.0f, 0.0f);
        
        glPushMatrix();
        glScalef(0.4, 0.4, 1.25f);
        glutSolidSphere(size, 100, 100);
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.0, 0.0, -15.0);
        glScaled(0.1, 0.1, 1.0);
        glutSolidTorus(size, size, 100, 100);
        glPopMatrix();


        //left
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(20 - wingLeft, 0.0, 0.0, 1.0);
        glTranslatef(50.0, 5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
        glTranslatef(55.0, 0.0, 0.0);
        glScaled(0.075, 0.075, 1.0);
        glutSolidTorus(size-30.0, size, 100, 100);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(-20 + wingRight, 0.0, 0.0, 1.0);
        glTranslatef(50.0, -5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
        glTranslatef(55.0, 0.0, 0.0);
        glScaled(0.075, 0.075, 1.0);
        glutSolidTorus(size - 30.0, size, 100, 100);
        glPopMatrix();

        ////right
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(20 - wingLeft, 0.0, 0.0, 1.0);
        glTranslatef(-50.0, -5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
        glTranslatef(-55.0, 0.0, 0.0);
        glScaled(0.075, 0.075, 1.0);
        glutSolidTorus(size - 30.0, size, 100, 100);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(-20 + wingRight, 0.0, 0.0, 1.0);
        glTranslatef(-50.0, 5.0, -30.0);
            glPushMatrix();
            glScalef(2.0f, 0.1, 0.6);
            glutSolidCube(size);
            glPopMatrix();
        glTranslatef(-55.0, 0.0, 0.0);
        glScaled(0.075, 0.075, 1.0);
        glutSolidTorus(size - 30.0, size, 100, 100);
        glPopMatrix();

        glPopMatrix();
    }
    for (auto& particle : particles)
    {
        float size = particle->size;
        glPushMatrix();
        glTranslatef(particle->position.x, particle->position.y, particle->position.z);
        glRotatef(-player->horRadian * 180 / PI, 0.0, 1.0, 0.0);
        glRotatef(player->verRadian * 180 / PI, 1.0f, 0.0f, 0.0f);
        glDisable(GL_LIGHTING);
        glBegin(GL_QUADS);
        glColor4f(particle->color[0], particle->color[1], particle->color[2], particle->color[3]);
        glVertex3f(-size, -size, 0.0);
        glVertex3f(-size, size, 0.0);
        glVertex3f(size, size, 0.0);
        glVertex3f(size, -size, 0.0);
        glEnd();
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void drawBullet(Bullet* bullet)
{
    float size = bullet->size;
    glPushMatrix();
    glTranslatef(bullet->position.x, bullet->position.y, bullet->position.z);
    glRotatef(-player->horRadian * 180 / PI, 0.0, 1.0, 0.0);
    glRotatef(player->verRadian * 180 / PI, 1.0f, 0.0f, 0.0f);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor3f(bullet_color[0], bullet_color[1], bullet_color[2]);
    glVertex3f(-size, -size, 0.0);
    glVertex3f(-size, size, 0.0);
    glVertex3f(size, size, 0.0);
    glVertex3f(size, -size, 0.0);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawAsteroid(Asteroid* asteroid)
{
    float mat_colour[] = { asteroid_color[0], asteroid_color[1], asteroid_color[2], 1.0 };
    float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    float mat_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_colour);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glPushMatrix();
    glTranslatef(asteroid->position.x, asteroid->position.y, asteroid->position.z);
    glRotatef(asteroid->asteroidRotateRadian * 180 / PI, asteroid->velocity.x, 0.0, 0.0);
    glutSolidSphere(asteroid->radius, 10, 10);
    for (int i = 0; i < asteroid->vertices; i++)
    {
        glPushMatrix();
        glRotatef(asteroid->offsetsHor[i] * 180 / PI, 0.0, asteroid->velocity.y, 0.0);
        glRotatef(asteroid->offsetsVer[i] * 180 / PI, asteroid->velocity.x, 0.0, 0.0);
        glTranslatef(asteroid->radius - asteroid->radius / 2, asteroid->radius - asteroid->radius / 2, asteroid->radius - asteroid->radius / 2);
        glutSolidSphere(asteroid->radius / 2, 10, 10);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawExplosion(Explosion* explosion)
{
    float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    float mat_shininess[] = { 100.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, explosion->color);
    float distance = magnitude(player->position - explosion->position);
    for (auto& particle : explosion->particles)
    {       
        float size = particle->size * distance / NORM_DISTANCE;
        glPushMatrix();
        glTranslatef(particle->position.x, particle->position.y, particle->position.z);
        glRotatef(-player->horRadian * 180 / PI, 0.0, 1.0, 0.0);
        glRotatef(player->verRadian * 180 / PI, 1.0f, 0.0f, 0.0f);
        glDisable(GL_LIGHTING);
        glBegin(GL_QUADS);
        glColor4f(particle->color[0], particle->color[1], particle->color[2], particle->color[3]);
        glVertex3f(-size, -size, 0.0);
        glVertex3f(-size, size, 0.0);
        glVertex3f(size, size, 0.0);
        glVertex3f(size, -size, 0.0);
        glEnd();
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
    
}

void place_camera()
{
    gluLookAt(camera->position.x, camera->position.y, camera->position.z,
       (double)camera->position.x + camera->front.x, (double)camera->position.y + camera->front.y, (double)camera->position.z + camera->front.z,
        camera->up.x, camera->up.y, camera->up.z);
}

void update_camera_position(float dt)
{
    camera->updateCameraVectors();
}

void render_frame()
{
    drawCrosshair();
    drawFPS(std::to_string(static_cast<int>(lastFPS)));
    if (gameStarted && !playerDied)
    {
        drawPlayerInfo(player);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    place_camera();
    drawArena();
    if (gameStarted)
    {
        if (playerDied)
        {
            drawDieMessage();
            time_since_game_start = 0;
        }
        else
        {
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
    }
    else
    {
        drawStartMessage();
    }
}

void on_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    render_frame();
    glutSwapBuffers();
    int err;
    while ((err = glGetError()) != GL_NO_ERROR) 
    {
        printf("Error: %s\n", gluErrorString(err));
    }
}

void on_reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera->getZoom(), screenWidth/screenHeight, 0.1f, (double)arenaSize * 3.0f);
}

void on_mouse_button(int button, int state, int x, int y)
{
    if (gameStarted && !playerDied)
    {
        if (button == 3)
        {
            camera->mouseScroll(true);
        }
        else if (button == 4)
        {
            camera->mouseScroll(false);
        }
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            mousePressed[0] = KeyState::PRESSED;
        }
        else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        {
            mousePressed[0] = KeyState::FREE;
        }
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            mousePressed[1] = KeyState::PRESSED;
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
        {
            mousePressed[1] = KeyState::FREE;
        }
    }
    else if (!gameStarted)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            gameStarted = true;
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            gameStarted = true;
        }
    }
    else if (playerDied)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            restartGame();
        }
        else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        {
            restartGame();
        }
    }
}

void on_key_press(unsigned char key, int x, int y)
{
    if (gameStarted && !playerDied) //work normally when game started (not in the beginning screen) and player still have some life left
    {
        if (key == KEY_ESC)
        {
            exit(EXIT_SUCCESS);
        }
        //i use the KeyState here so that it can detect multiple inputs at once 
        if (key == 'w' || key == 'W')
        {
            keyArray[int('w')] = KeyState::PRESSED;
        }
        if (key == 's' || key == 'S')
        {
            keyArray[int('s')] = KeyState::PRESSED;
        }
        if (key == 'a' || key == 'A')
        {
            keyArray[int('a')] = KeyState::PRESSED;
        }
        if (key == 'd' || key == 'D')
        {
            keyArray[int('d')] = KeyState::PRESSED;
        }
        if (key == 'f' || key == 'F')
        {
            keyArray[int('f')] = KeyState::PRESSED;
            camera->changeCamera();
        }
    }
    else if (!gameStarted) //this is at the begining of the game, press any keys to start the game
    {
        if (key == KEY_ESC)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            gameStarted = true;
        }
    }
    else if (playerDied) //this is when player have no life left, press any key to restart the game
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
}

void on_key_release(unsigned char key, int x, int y)
{
    if (key == 'w' || key == 'W')
    {
        keyArray[int('w')] = KeyState::FREE;
    }
    if (key == 's' || key == 'S')
    {
        keyArray[int('s')] = KeyState::FREE;
    }
    if (key == 'a' || key == 'A')
    {
        keyArray[int('a')] = KeyState::FREE;
    }
    if (key == 'd' || key == 'D')
    {
        keyArray[int('d')] = KeyState::FREE;
    }
    if (key == 'f' || key == 'F')
    {
        keyArray[int('f')] = KeyState::FREE;
    }
}

void playerMovementAndParticles(float dt)
{
    if (keyArray[int('a')] == KeyState::PRESSED && keyArray[int('d')] == KeyState::FREE)
    {
        player->rotateLeft();
        if (wingLeft < 40.0f)
        {
            wingLeft += 2.0f;
        }
        camera->yawVel = -10;
        //std::cout << camera->yawVel << std::endl;

    }
    else
    {
        if (wingLeft > 0.0f)
        {
            wingLeft -= 2.0f;
        }
       
    }
    if (keyArray[int('d')] == KeyState::PRESSED && keyArray[int('a')] == KeyState::FREE)
    {
        player->rotateRight();
        if (wingRight < 40.0f)
        {
            wingRight += 2.0f;
        }
    }
    else
    {
        if (wingRight > 0.0f)
        {
            wingRight -= 2.0f;
        } 
    }
    if (keyArray[int('w')] == KeyState::PRESSED)
    {
        if (camera->pitch + 35 <= 89.0f)
        {
            player->rotateUp();
        }
    }
   

    if (keyArray[int('s')] == KeyState::PRESSED)
    {
        if (camera->pitch + 25 >= -89.0f)
        {
            player->rotateDown();
        }
    }
    if (mousePressed[1] == KeyState::PRESSED)
    {       
        if (player->velocity.x <= MAX_SPEED)
        {
            player->velocity.x += player->thrustSpeed * -sin(player->horRadian) / currentFPS * cos(player->verRadian);
        }
        if (player->velocity.y <= MAX_SPEED)
        {
            player->velocity.y += player->thrustSpeed * -sin(player->verRadian) / currentFPS;
        }
        if (player->velocity.z <= MAX_SPEED)
        {
            player->velocity.z += player->thrustSpeed * cos(player->horRadian) / currentFPS * cos(player->verRadian);
        }
        if (camera->dynamic.x < MAX_SPEED)
        {
            camera->dynamic.x += player->thrustSpeed * -sin(player->horRadian) / currentFPS * cos(player->verRadian);
        }
        if (camera->dynamic.y < MAX_SPEED)
        {
            camera->dynamic.y += player->thrustSpeed * -sin(player->verRadian) / currentFPS;
        }
        if (camera->dynamic.z < MAX_SPEED)
        {
            camera->dynamic.z += player->thrustSpeed * cos(player->horRadian) / currentFPS * cos(player->verRadian);
        }
        isThrusting = true; //this is used to determine when to draw the particles
        //this is used to draw the particles coming out from the back of the ship
        if (particles.size() < particleNum)
        {
            if (particle_dt >= timeBetweenParticles) //this is to delaying the particle coming out
            {
                for (int i = 0; i < 2; i++)
                {
                    Particle* particle = new Particle(player, INIT_PARTICLE_SIZE, particle_color);
                    particles.push_back(particle);
                    particle->velocity.x = particleSpeed / currentFPS * -sin(particle->horRadian) * cos(particle->verRadian);
                    particle->velocity.y = particleSpeed / currentFPS * -sin(particle->verRadian);
                    particle->velocity.z = particleSpeed / currentFPS * cos(particle->horRadian) * cos(particle->verRadian);
                }
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
        //if player release the 'w' key, the player will slow down by FRICTION
        player->velocity = player->velocity * (1.0f - FRICTION / currentFPS);
        camera->dynamic = camera->dynamic * (1.0f - FRICTION / currentFPS);
    }
    player->update();
    
    if (player->position.x < -arenaSize || player->position.x > arenaSize ||
        player->position.y < -arenaSize || player->position.y > arenaSize ||
        player->position.z < -arenaSize || player->position.z > arenaSize)
    {
        if (!invulnerable_state)
        {
            //explosions.push_back(new Explosion(player->posX, player->posY, player->playerSize, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE * scale, scale));
            player->die();
            invulnerable_state = true;
        }
        else
        {
            player->position = vector3(0.0f, 0.0f, 0.0f);
        }
    }

    for (int i = 0; i < particles.size(); i++)
    {
        particles[i]->update();
        particles[i]->size -= dt * sizeReduceRate; //reduce the size base on the dt
        if (particles[i]->size <= 0.0f) //when they are smaller than 0, removed it from the vector of particles
        {
            delete particles[i];
            particles.erase(particles.begin() + i);
            break;
        }
    }
    warnings.clear(); //we want to clear the warnings vector everytime, so that the wall can be set back to WHITE when player is not near the wall
        //I didnt use else here so that there can be multiple warnings at the same time
    if (player->position.x < -arenaSize +  WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::RIGHT);
    }
    if (player->position.x > arenaSize - WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::LEFT);
    }
    if (player->position.y < -arenaSize + WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::BOTTOM);
    }
    if (player->position.y > arenaSize - WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::TOP);
    }
    if (player->position.z < -arenaSize + WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::BACK);
    }
    if (player->position.z > arenaSize - WARNING_DISTANCE)
    {
        warnings.push_back(WallWarning::FRONT);
    }
    //check when player have no life left
    if (player->life < 0)
    {
        playerDied = true;
    }
    //this is used to manage the invulnerable state of the player via dt
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

void bulletSpawnAndMovement(float dt)
{
    ////delete the bullet that are out of the wall
    for (int i = 0; i < bullets.size(); i++)
    {
        if (bullets[i]->position.x < -arenaSize || bullets[i]->position.x > arenaSize ||
            bullets[i]->position.y < -arenaSize || bullets[i]->position.y > arenaSize ||
            bullets[i]->position.z < -arenaSize || bullets[i]->position.z > arenaSize)
        {
            delete bullets[i];
            bullets.erase(bullets.begin() + i);
            break;
        }
    }

    ////delete the bullet that hit the asteroid
    for (int i = 0; i < bullets.size(); i++)
    {
        bullets[i]->move();
        for (auto& asteroid : asteroids)
        {
            if (magnitude(bullets[i]->position - asteroid->position) <= (asteroid->radius + bullets[i]->size))
            {
                asteroid->getHit();
                //asteroid only get hit and lose hp here, asteroid will be deleted when its hp is 0
                delete bullets[i];
                bullets.erase(bullets.begin() + i);
                break;
            }
        }
    }

    //this is used to limit the firing rate  
    shoot_delay += dt;
    if (shoot_delay >= TIME_DELAY_BETWEEN_TWO_SHOTS)
    {
        shoot_delay = 0.0f;
        shootable = true;
    }

    if (mousePressed[0] == KeyState::PRESSED && shootable == true)
    {
        shootable = false;
        shoot_delay = 0.0f;

        Bullet* bullet = new Bullet(player);
        bullet->velocity.x += -sin(bullet->horRadian) * bullet->speed / currentFPS * cos(bullet->verRadian);
        bullet->velocity.z += cos(bullet->horRadian) * bullet->speed / currentFPS * cos(bullet->verRadian);
        bullet->velocity.y += -sin(bullet->verRadian) * bullet->speed / currentFPS;
        bullets.push_back(bullet);
    }
}

void asteroidSpawnAndMovement(float dt)
{
    //this is used to limit the number of asteroids per round as well as the delaying between the spawning of 2 asteroids
    asteroid_spawn_delay += dt;
    if (asteroid_spawn_delay >= asteroid_spawn_rate && asteroids_number > 0)
    {
        asteroids_number--;
        int side1 = rand() % 3;
        int side2 = rand() % 2;
        float asteroidX, asteroidY, asteroidZ;
        if (side1 == 0)
        {
            if (side2 == 0)
            {
                asteroidX = (float)(rand() % 1000 + arenaSize + 2000);
            }
            else
            {
                asteroidX = (float)(rand() % 1000 - arenaSize - 3000);
            }
            asteroidY = (float)(rand() % (int)((arenaSize + 3000) * 2) - (arenaSize + 3000));
            asteroidZ = (float)(rand() % (int)((arenaSize + 3000) * 2) - (arenaSize + 3000));
        }
        else if (side1 == 1)
        {
            if (side2 == 0)
            {
                asteroidY = (float)(rand() % 1000 + arenaSize + 2000);
            }
            else
            {
                asteroidY = (float)(rand() % 1000 - arenaSize - 3000);
            }
            asteroidX = (float)(rand() % (int)((arenaSize + 3000) * 2) - (arenaSize + 3000));
            asteroidZ = (float)(rand() % (int)((arenaSize + 3000) * 2) - (arenaSize + 3000));
        }
        else
        {
            if (side2 == 0)
            {
                asteroidZ = (float)(rand() % 1000 + arenaSize + 1000);
            }
            else
            {
                asteroidZ = (float)(rand() % 1000 - arenaSize - 2000);
            }
            asteroidY = (float)(rand() % (int)((arenaSize + 2000) * 2) - (arenaSize + 2000));
            asteroidX = (float)(rand() % (int)((arenaSize + 2000) * 2) - (arenaSize + 2000));
        }
        
        float asteroid_radius = (float)(rand() % ASTEROID_SIZE + ASTEROID_SIZE);
        Asteroid* asteroid = new Asteroid(vector3(asteroidX, asteroidY, asteroidZ), vector3(player->position.x, player->position.y, player->position.z), asteroid_radius);
        float velLength = magnitude(asteroid->target - asteroid->position);
        asteroid->velocity = (asteroid->target - asteroid->position) / (velLength / asteroid->asteroidSpeed) / currentFPS;
        asteroids.push_back(asteroid);
        asteroid_spawn_delay = 0.0;
    }

    //if the asteroids_number is 0 and the size of the vector is 0, go to next rounds with new asteroids_number 
    if (asteroids_number == 0 && asteroids.size() == 0)
    {
        currRound++;
        asteroids_number = INIT_ASTEROIDS_NUMBER * currRound;
    }

    //this is used to calculate the movement of the asteroids when touching the wall or touching each other
    for (auto& asteroid : asteroids)
    {
        //when touching the wall, just need to change the side of the velocity
        if (asteroid->position.x > arenaSize - asteroid->radius && asteroid->velocity.x > 0)
        {
            asteroid->velocity.x *= -1;
        }
        else if (asteroid->position.x < -arenaSize + asteroid->radius && asteroid->velocity.x < 0)
        {
            asteroid->velocity.x *= -1;
        }

        if (asteroid->position.y > arenaSize - asteroid->radius && asteroid->velocity.y > 0)
        {
            asteroid->velocity.y *= -1;
        }
        else if (asteroid->position.y < -arenaSize + asteroid->radius && asteroid->velocity.y < 0)
        {
            asteroid->velocity.y *= -1;
        }

        if (asteroid->position.z > arenaSize - asteroid->radius && asteroid->velocity.z > 0)
        {
            asteroid->velocity.z *= -1;
        }
        else if (asteroid->position.z < -arenaSize + asteroid->radius && asteroid->velocity.z < 0)
        {
            asteroid->velocity.z *= -1;
        }
        //this is used to calculate the movement of 2 asteroids when they touch each other, base on elastic collison
        for (auto& asteroid2 : asteroids)
        {
            if (asteroid2 != asteroid)
            {
                vector3 distance = vector3(asteroid->position.x - asteroid2->position.x, asteroid->position.y - asteroid2->position.y, asteroid->position.z - asteroid2->position.z);
                float d = magnitude(distance);
                if (d <= asteroid2->radius + asteroid->radius)
                {
                    float overlap = 0.5f * (d - asteroid->radius - asteroid2->radius);
                    asteroid->position.x -= overlap * (asteroid->position.x - asteroid2->position.x) / d;
                    asteroid->position.y -= overlap * (asteroid->position.y - asteroid2->position.y) / d;
                    asteroid->position.z -= overlap * (asteroid->position.z - asteroid2->position.z) / d;
                    asteroid2->position.x += overlap * (asteroid->position.x - asteroid2->position.x) / d;
                    asteroid2->position.y += overlap * (asteroid->position.y - asteroid2->position.y) / d;
                    asteroid2->position.z += overlap * (asteroid->position.z - asteroid2->position.z) / d;
                    vector3 centerToCenter1 = asteroid->position - asteroid2->position;
                    vector3 centerToCenter2 = asteroid2->position - asteroid->position;
                    asteroid->velocity = (centerToCenter1 + asteroid->velocity) / magnitude(centerToCenter1 + asteroid->velocity) * magnitude(asteroid->velocity);
                    asteroid2->velocity = (centerToCenter2 + asteroid2->velocity) / magnitude(centerToCenter2 + asteroid2->velocity) * magnitude(asteroid2->velocity);
                }
            }
        }

        //this is used to prevent the asteroid going crazy 
        if (asteroid->velocity.x > ASTEROID_MAX_VELOCITY)
        {
            asteroid->velocity.x = ASTEROID_MAX_VELOCITY;
        }
        if (asteroid->velocity.y > ASTEROID_MAX_VELOCITY)
        {
            asteroid->velocity.y = ASTEROID_MAX_VELOCITY;
        }
        if (asteroid->velocity.z > ASTEROID_MAX_VELOCITY)
        {
            asteroid->velocity.z = ASTEROID_MAX_VELOCITY;
        }
        asteroid->velocity.x -= ASTEROID_FRICTION * asteroid->velocity.x / currentFPS;
        asteroid->velocity.y -= ASTEROID_FRICTION * asteroid->velocity.y / currentFPS;
        asteroid->velocity.z -= ASTEROID_FRICTION * asteroid->velocity.z / currentFPS;
        //add velocity to position
        asteroid->update();
    }

    //this is used to check if any asteroids touch the player by check the distance
    for (int i = 0; i < asteroids.size(); i++)
    {
        if (magnitude(player->position - asteroids[i]->position) <= (asteroids[i]->radius + 80.0))
        {
            if (!invulnerable_state)
            {
                explosions.push_back(new Explosion(player->position, player, 80.0 * 1.5f, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
                player->die();
                invulnerable_state = true;
            }
        }
    }

    //this is calculate the splitting effect and 2 small asteroids' movement after being splitted from the big asteroid
    for (int j = 0; j < asteroids.size(); j++)
    {
        if (asteroids[j]->hp <= 0) //the asteroid only destroyed when its hp below 0
        {
            player->score += (int)(asteroids[j]->radius); //increase the score of the player by the radius of asteroid
            player->asteroidsDestroyed++;

            if (asteroids[j]->state == AsteroidState::SPLITTABLE) //asteroid can only be splitted once, so is this used to check if the asteroid is splittable
            {
                float r1 = asteroids[j]->asteroidHorRadian - PI * 0.25f;
                float r2 = asteroids[j]->asteroidHorRadian + PI * 0.25f;
                float x1 = asteroids[j]->position.x - sin(r1) * asteroids[j]->radius;
                float z1 = asteroids[j]->position.z + cos(r1) * asteroids[j]->radius;
                float x2 = asteroids[j]->position.x - sin(r2) * asteroids[j]->radius;
                float z2 = asteroids[j]->position.z + cos(r2) * asteroids[j]->radius;
                //make 2 asteroid that is unsplittable
                Asteroid* a1 = new Asteroid(vector3(x1, asteroids[j]->position.y, z1), asteroids[j]->target, asteroids[j]->radius * 0.5f, asteroids[j]->asteroidSpeed, AsteroidState::UNSPLITTABLE);
                Asteroid* a2 = new Asteroid(vector3(x2, asteroids[j]->position.y, z2), asteroids[j]->target, asteroids[j]->radius * 0.5f, asteroids[j]->asteroidSpeed, AsteroidState::UNSPLITTABLE);
                a1->velocity = asteroids[j]->velocity;
                a2->velocity = asteroids[j]->velocity;
                a1->asteroidHorRadian = r1;
                a2->asteroidHorRadian = r2;
                a1->velocity.x = asteroids[j]->velocity.x / sin(asteroids[j]->asteroidHorRadian) * sin(a1->asteroidHorRadian);
                a1->velocity.z = asteroids[j]->velocity.z / cos(asteroids[j]->asteroidHorRadian) * cos(a1->asteroidHorRadian);
                a2->velocity.x = asteroids[j]->velocity.x / sin(asteroids[j]->asteroidHorRadian) * sin(a2->asteroidHorRadian);
                a2->velocity.z = asteroids[j]->velocity.z / cos(asteroids[j]->asteroidHorRadian) * cos(a2->asteroidHorRadian);
                asteroids.push_back(a1);
                asteroids.push_back(a2);
            }
            ////make an explosion at the big asteroid location
            explosions.push_back(new Explosion(asteroids[j]->position, player, asteroids[j]->radius * 1.5f, explosion_color, explosionParticles, EXPLOSION_PARTICLE_SIZE));
            delete asteroids[j];
            asteroids.erase(asteroids.begin() + j);
            break;
        }
    }
}

void explosionEffect(float dt)
{
    for (int i = 0; i < explosions.size(); i++)
    {
        for (int j = 0; j < explosions[i]->particles.size(); j++)
        {
            explosions[i]->particles[j]->update();
            explosions[i]->particles[j]->size -= dt * explosions[i]->sizeReduceRate ; //make the particle smaller with dt
            if (explosions[i]->particles[j]->size <= 0.0) //if the particle is smaller than 0, it will be deleted
            {
                delete explosions[i]->particles[j];
                explosions[i]->particles.erase(explosions[i]->particles.begin() + j);
                break;
            }
        }
        if (explosions[i]->particles.size() == 0) //when all the particles are deleted, the explosion will be deleted and removed from the vector
        {
            delete explosions[i];
            explosions.erase(explosions.begin() + i);
            break;
        }
    }
}

void update_game_state(float dt)
{
    playerMovementAndParticles(dt);
    bulletSpawnAndMovement(dt);
    asteroidSpawnAndMovement(dt);
    explosionEffect(dt);
}

void on_idle()
{
    ShowCursor(FALSE);
    float t = (float)glutGet(GLUT_ELAPSED_TIME);
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
    update_camera_position(dt);
    glutPostRedisplay();
}

void init_lighting()                             
{

    float ambient0[] = {1.0, 1.0, 1.0, 1.0};
    float diffuse0[] = {1.0, 1.0, 1.0, 1.0};
    float specular0[] = {1.0, 1.0, 1.0, 1.0};

    glShadeModel(GL_FLAT);

    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glEnable(GL_LIGHT0);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    screenWidth = (float)glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = (float)glutGet(GLUT_SCREEN_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 2");
    glutFullScreen();
    player = new Player(0.0f, 0.0f, 0.0f);
    camera = new Camera(200.0f, 8.0f, 200.0f, 0.0f, 1.0f, 0.0f, player);
    srand(time(NULL));
    glutReshapeFunc(on_reshape);

    glutMouseFunc(on_mouse_button);

    glutKeyboardFunc(on_key_press);
    glutKeyboardUpFunc(on_key_release);
    glutDisplayFunc(on_display);
    glutIdleFunc(on_idle);
    init_lighting();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);    
    glutMainLoop();
    return(EXIT_SUCCESS);
}

