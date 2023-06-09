#pragma once
// Room.h

#ifndef ROOM_H
#define ROOM_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "globals.h"

class Room {
	int height;
	float floorH;

public:
	Room(int h, float fh);
	void drawPlate(glm::mat4 Mb, glm::vec3 coords, glm::vec3 cubeScal, GLuint tex, GLuint texSpec, bool reflected, float rotateAngle = 0);
	void drawWall(bool rotated, glm::mat4 M, float var, GLuint tex, GLuint texSpec, glm::vec3 plateScal, float range, bool reflected, wallType wt);
};

#endif
