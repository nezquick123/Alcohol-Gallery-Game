#include "Floor.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include "constants.h"
#include "textures.h"
#include "globals.h"
Floor::Floor() {
    plateScalNotRot = glm::vec3(1.0f, 1.0f, 0.125f);
    plateScalRot = glm::vec3(0.125f, 1.0f, 1.0f);
    floorScaleVec = glm::vec3(10.0f, 0.125f, 10.0f);
    height = 5;
    roomNum = 5;
}

void Floor::draw(float offset, GLuint tex0, GLuint tex1, GLuint texSpecWall) {
	bool rotFlag = false;
	if (offset > 0.0f) //negative offset means that rooms need to be rotated by 180 degrees around Y axis
	{
		rotFlag = true;
		offset *= -1;
	}

	for (int rn = 0; rn <= roomNum - 1; rn++) { //room number
		int roomCoord = -50 + 20 * rn;
		glm::mat4 Ms = glm::mat4(1.0f);
		if (rotFlag) //rotate and correct x offset
		{
			Ms = glm::rotate(Ms, PI, glm::vec3(0.0f, 1.0f, 0.0f));
			Ms = glm::translate(Ms, glm::vec3(20.0f, 0.0f, offset));
		}
		Ms = glm::translate(Ms, glm::vec3((float)roomCoord, 0.0f, offset));
		Room wall(height, floorScaleVec.y);
		wall.drawWall(0, Ms, floorScaleVec.x, tex0, texSpecWall, plateScalNotRot, floorScaleVec.x, rotFlag, DOOR);
		wall.drawWall(0, Ms, -floorScaleVec.x, tex0, texSpecWall, plateScalNotRot, floorScaleVec.x, rotFlag, WINDOWS);
		wall.drawWall(1, Ms, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, rotFlag, BASIC);
		wall.drawWall(1, Ms, -floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, rotFlag, BASIC);

		//floor
		for (int i = 0; i < 2; i++) {
			glm::mat4 Mp = glm::translate(Ms, glm::vec3(0.0f, i * height * plateScalRot.y * 2, 0.0f));//ceiling if i == 1
			Mp = glm::scale(Mp, floorScaleVec);
			textureCube(Mp, tex1, lpmain, false);
		}

		//corridor
		if (rotFlag) { //just for one side
			glm::mat4 Mc = glm::mat4(1.0f);
			Mc = glm::translate(Mc, glm::vec3((float)roomCoord, 0.0f, 0.0f));
			if (rn == 0) {
				glm::mat4 Mw = glm::translate(Mc, glm::vec3(-20.0f, 0.0f, 0.0f));
				wall.drawWall(1, Mw, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, !rotFlag, DOOR);
			}
			if (rn == roomNum - 1) {
				wall.drawWall(1, Mc, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, !rotFlag, WINDOWS);
			}
			for (int i = 0; i < 2; i++) {
				Mc = glm::translate(Mc, glm::vec3(0.0f, i * height * plateScalRot.y * 2, 0.0f));//ceiling if i == 1
				Mc = glm::scale(Mc, floorScaleVec);
				textureCube(Mc, tex1, lpmain, false);
				Mc = glm::translate(glm::mat4(1.0f), glm::vec3((float)roomCoord, 0.0f, 0.0f));
			}
		}
	}
}
