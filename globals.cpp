#include "globals.h"
#include "myCube.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

float* vertices = myCubeVertices;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
float* normals = myCubeNormals;
int vertexCount = myCubeVertexCount;
glm::vec4 lpmain = glm::vec4(0, 9, 0, 1);
ShaderProgram* sp;
enum wallType { BASIC, WINDOWS, DOOR };
GLuint tex0;
GLuint tex1;
GLuint skytex;
GLuint woodtex;
GLuint orangeGlass;
GLuint texSpecWall;
GLuint texSpecFloor;
float drunk_coef = 0.0f;
float sinarg = 0;