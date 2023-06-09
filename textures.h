#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

void textureCube(glm::mat4 M, GLuint tex, glm::vec4 lp, bool inside);
void textureCubeSpec(glm::mat4 M, GLuint tex, GLuint texSpec, glm::vec4 lp, bool inside);
