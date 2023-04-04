#pragma once
#include "Utility.h"
enum class cameraMovement
{
	UP,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW = 90.0f;
const GLfloat PITCH = -30.0f;
const GLfloat SPEED = 0.1f;
const GLfloat SENSITIVTY = 0.1f;
const GLfloat ZOOM = 100.0f;
const GLfloat DISTANCE = 200;
class Camera
{
public:
	vector3 position, up, front, right;
	float distance = DISTANCE;
	float pitch, yaw, movementSpeed, mouseSensitivity, zoom, lastX, lastY;
	float velocity = 0.0f;
	bool firstMouse = true;
	Player* player;
	vector3 dynamic;
	boolean mode[4];
	float yawVel = 0.0;
	Camera(float posX, float posY, float posZ,
		float upX, float upY, float upZ, Player * player)
	{
		this->front = vector3(0.0f, 0.0f,-1.0f);
		this->player = player;
		position = vector3(posX, posY, posZ);
		//position = player->front;
		up = vector3(upX, upY, upZ);
		this->pitch = PITCH;
		this->yaw = YAW;
		this->movementSpeed = SPEED;
		this->mouseSensitivity = SENSITIVTY;
		this->zoom = ZOOM;
		dynamic = vector3(0.0, 0.0, 0.0);
		mode[0] = true;
		mode[1] = false;
		mode[2] = false;
		mode[3] = false;
	}

	~Camera()
	{

	}


	void mouseScroll(bool zoomIn)
	{
		if (this->zoom >= ZOOM * 0.5f && this->zoom <= ZOOM)
		{
			if (zoomIn)
			{
				this->zoom--;
			}
			else
			{
				this->zoom++;
			}
		}

		if (this->zoom <= ZOOM * 0.5f)
		{
			this->zoom = ZOOM * 0.5f;
		}

		if (this->zoom >= ZOOM)
		{
			this->zoom = ZOOM;
		}
	}

	GLfloat getZoom()
	{
		return this->zoom;
	}

	//change camera angle
	void changeCamera()
	{
		if (mode[0] == true)
		{
			mode[0] = false;
			mode[1] = true;
			mode[2] = false;
			mode[3] = false;
		}
		else if (mode[1] == true)
		{
			mode[0] = false;
			mode[1] = false;
			mode[2] = true;
			mode[3] = false;
		}
		else if (mode[2] == true)
		{
			mode[0] = false;
			mode[1] = false;
			mode[2] = false;
			mode[3] = true;
		}
		else
		{
			mode[0] = true;
			mode[1] = false;
			mode[2] = false;
			mode[3] = false;
		}
	}
	void updateCameraVectors()
	{
		if (mode[0])
		{
			this->pitch = PITCH - player->verRadian * 180 / PI;
			float distanceHor = cos(angleToRadian(this->pitch)) * distance;
			float distanceVer = sin(angleToRadian(this->pitch)) * distance;
			this->position.y = player->position.y - distanceVer;
			float theta = player->horRadian * (180 / PI);
			float Xoffset = distanceHor * sin(angleToRadian(theta));
			float Zoffset = distanceHor * cos(angleToRadian(theta));
			this->position.x = (player->position.x + Xoffset);
			this->position.z = (player->position.z - Zoffset);
			this->position = this->position - this->dynamic;
			this->yaw = (YAW + player->horRadian * (180 / PI));
			vector3 front;
			front.x = cos(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			front.y = sin(angleToRadian(this->pitch + 30));
			front.z = sin(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			this->front = normalize(front);
			this->right = normalize(cross(this->front, this->up));
		}
		else if (mode[1])
		{
			this->pitch = PITCH - player->verRadian * 180 / PI;
			float distanceHor = cos(angleToRadian(this->pitch)) * distance;
			float distanceVer = sin(angleToRadian(this->pitch)) * distance;
			this->position.y = player->position.y - distanceVer;
			float theta = player->horRadian * (180 / PI);
			float Xoffset = distanceHor * sin(angleToRadian(theta));
			float Zoffset = distanceHor * cos(angleToRadian(theta));
			this->position.x = (player->position.x + Xoffset);
			this->position.z = (player->position.z - Zoffset);
			this->position = this->position - this->dynamic;
			this->yaw = (YAW + player->horRadian * (180 / PI)) - 90;
			vector3 front;
			front.x = cos(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			front.y = sin(angleToRadian(this->pitch + 30));
			front.z = sin(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			this->front = normalize(front);
			this->right = normalize(cross(this->front, this->up));
		}
		else if (mode[2])
		{
			this->pitch = PITCH - player->verRadian * 180 / PI;
			float distanceHor = cos(angleToRadian(this->pitch)) * distance;
			float distanceVer = sin(angleToRadian(this->pitch)) * distance;
			this->position.y = player->position.y - distanceVer;
			float theta = player->horRadian * (180 / PI);
			float Xoffset = distanceHor * sin(angleToRadian(theta));
			float Zoffset = distanceHor * cos(angleToRadian(theta));
			this->position.x = (player->position.x + Xoffset);
			this->position.z = (player->position.z - Zoffset);
			this->position = this->position - this->dynamic;
			this->yaw = (YAW + player->horRadian * (180 / PI)) + 90;
			vector3 front;
			front.x = cos(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			front.y = sin(angleToRadian(this->pitch + 30));
			front.z = sin(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			this->front = normalize(front);
			this->right = normalize(cross(this->front, this->up));
		}
		else 
		{
			this->pitch = PITCH - player->verRadian * 180 / PI;
			float distanceHor = cos(angleToRadian(this->pitch)) * distance;
			float distanceVer = sin(angleToRadian(this->pitch)) * distance;
			this->position.y = player->position.y - distanceVer;
			float theta = player->horRadian * (180 / PI);
			float Xoffset = distanceHor * sin(angleToRadian(theta));
			float Zoffset = distanceHor * cos(angleToRadian(theta));
			this->position.x = (player->position.x + Xoffset);
			this->position.z = (player->position.z - Zoffset);
			this->position = this->position - this->dynamic;
			this->yaw = (YAW + player->horRadian * (180 / PI)) + 180;
			vector3 front;
			front.x = cos(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			front.y = sin(angleToRadian(this->pitch + 30));
			front.z = sin(angleToRadian(this->yaw)) * cos(angleToRadian(this->pitch + 30));
			this->front = normalize(front);
			this->right = normalize(cross(this->front, this->up));
		}
	}
};

