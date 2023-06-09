#pragma once
#include <glm/glm.hpp>
#include "Room.h" // Assuming Room class is defined in another header file

class Floor {
private:
    glm::vec3 plateScalNotRot;
    glm::vec3 plateScalRot;
    glm::vec3 floorScaleVec;
    int height;
    int roomNum;

public:
    Floor();
    void draw(float offset, GLuint tex0, GLuint tex1, GLuint texSpecWall);
};
