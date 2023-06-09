#pragma once
#include "shaderprogram.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

extern float* vertices;
extern float* texCoords;
extern float* colors;
extern float* normals;
extern int vertexCount;
extern glm::vec4 lpmain;
extern ShaderProgram* sp;

extern GLuint tex0;
extern GLuint tex1;
extern GLuint skytex;
extern GLuint woodtex;
extern GLuint orangeGlass;
extern GLuint texSpecWall;
extern GLuint texSpecFloor;
extern float drunk_coef;
extern float sinarg;
extern enum wallType;