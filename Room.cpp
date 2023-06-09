// Room.cpp

#include "Room.h"
#include "globals.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include "textures.h"

Room::Room(int h, float fh) {
	height = h;
	floorH = fh;
}

void Room::drawPlate(glm::mat4 Mb, glm::vec3 coords, glm::vec3 cubeScal, GLuint tex, GLuint texSpec, bool reflected, float rotateAngle) {
	// Implementation code...
	float drunkfun = drunk_coef * sin(drunk_coef * 2 * sinarg) + 1.5 * drunk_coef;// +drunk_coef;'
	float scalx = 1.0f, scalz = 1.0f;
	if (drunk_coef != 0.0f) {

		if (reflected) {
			drunkfun *= -1;

		}
		if (rotateAngle != 0) {
			scalx = abs(drunkfun) * 5;
		}
		else
			scalz = abs(drunkfun) * 5;
	}
	glm::mat4 Mp = glm::rotate(Mb, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	Mp = glm::translate(Mb, glm::vec3(coords.x + cubeScal.x + drunkfun, floorH + coords.y * 2 * cubeScal.y, coords.z + cubeScal.z + drunkfun));
	Mp = glm::scale(Mp, glm::vec3(cubeScal.x * scalx, cubeScal.y, cubeScal.z * scalz));
	textureCubeSpec(Mp, tex, texSpec, lpmain, false);
}

void Room::drawWall(bool rotated, glm::mat4 M, float var, GLuint tex, GLuint texSpec, glm::vec3 plateScal, float range, bool reflected, wallType wt) {
	// Implementation code...
	glm::vec3 plateCoords;
	int min, max;
	int j = 0;


	if (rotated) {//z axis
		for (float h = 0; h <= height; h += 1) {
			for (float i = -range, j = 0; i < range; i += 2 * plateScal.z, j++) {
				if (wt == DOOR) {
					if (!(h >= 0 && h < 5 && j >= 6 && j <= 8))
						drawPlate(M, glm::vec3(var, h, i), plateScal, tex, texSpec, reflected, 90);
				}
				else if (wt == WINDOWS) {
					if (!(h >= 2 && h <= 4 && ((j >= 1 && j <= 3) || (j >= 6 && j <= 8))))
						drawPlate(M, glm::vec3(var, h, i), plateScal, tex, texSpec, reflected, 90);
				}
				else
					drawPlate(M, glm::vec3(var, h, i), plateScal, tex, texSpec, reflected, 90);
			}
		}
	}
	else {
		for (float h = 0; h <= height; h += 1) {
			for (float i = -range, j = 0; i < range; i += 2 * plateScal.x, j++) {
				if (wt == DOOR) {
					if (!(h >= 0 && h < 5 && j >= 6 && j <= 8))
						drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, texSpec, reflected);
				}
				else if (wt == WINDOWS) {
					if (!(h >= 2 && h <= 4 && ((j >= 1 && j <= 3) || (j >= 6 && j <= 8))))
						drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, texSpec, reflected);
				}
				else
					drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, texSpec, reflected);
			}

		}
	}
}
