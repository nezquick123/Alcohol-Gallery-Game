/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

ATTRIBUTIONS:
wood texture: https://www.freepik.com/free-photo/wooden-textured-background_2768392.htm#query=wood%20texture&position=2&from_view=search&track=ais

*/



#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <GL/gl.h>
#include <GL/GLU.h>
#include <windows.h>
#include <SFML/Audio.hpp>
void textureCube(glm::mat4 M, GLuint tex, glm::vec4 lp, bool inside=false);
void textureCubeSpec(glm::mat4 M, GLuint tex, GLuint texSpec, glm::vec4 lp, bool inside = false);
void drinkingAnimation();
GLuint tex0;
GLuint tex1;
GLuint skytex;
GLuint woodtex;
GLuint orangeGlass;
GLuint deadFloor;
GLuint stainedGlass;
GLuint texSpecWall;
GLuint texSpecFloor;

std::vector<glm::mat4> modelPos;
glm::vec3 cameraPos = glm::vec3(0.0f, 7.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 tempCam = cameraPos;
float moveSpeedx = 0;
float moveSpeedz = 0;
ShaderProgram* sp; //Pointer to the shader program
glm::vec4 lpmain = glm::vec4(0, 9, 0, 1); //light position, world space


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 900.0f / 2.0;
float lastY = 900.0f / 2.0;
float fov = 45.0f;
float drunk_coef = 0.0f; //test conflict
byte drunkLevel = 0;
bool drinkUp = false;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

GLuint readTexture(const char* filename) { //global declaration
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	//Read into computers memory
	std::vector<unsigned char> image; //Allocate memory
	unsigned width, height; //Variables for image size
	//Read the image
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import to graphics card memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate handle
	//Copy image to graphics cards memory reprezented by the active handle
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //set mip mapping algorithm
	glGenerateMipmap(GL_TEXTURE_2D);
	return tex;
}

float* vertices = myCubeVertices;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
float* normals = myCubeNormals;
int vertexCount = myCubeVertexCount;
bool close = false;
//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}



float defaultSpeed = 0.8f;
//Procedura obsługi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) moveSpeedx = -defaultSpeed;
		if (key == GLFW_KEY_S) moveSpeedx = defaultSpeed;
		if (key == GLFW_KEY_A) moveSpeedz = -defaultSpeed;
		if (key == GLFW_KEY_D) moveSpeedz = defaultSpeed;
		if (key == GLFW_KEY_ESCAPE) close = true;
		if (key == GLFW_KEY_E) drinkUp = true;
		if (key == GLFW_KEY_F) drunkLevel = 10.0f;
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_S || key == GLFW_KEY_W) moveSpeedx = 0;
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) moveSpeedz = 0;
		if (key == GLFW_KEY_E) drinkUp = false;
	}
}




//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	delete sp;
	//************Place any code here that needs to be executed once, after the main loop ends************
}


class Collider {
	glm::vec4 coord; //x1, x2, z1, z2
	glm::mat3 triangs[4]; //boundary parallel to: ox, oy, ox, oy
	glm::vec4 edges[4]; //boundary parallel to: ox, oy, ox, oy
	glm::vec3 center = glm::vec3(0,0, 0);

public:
	Collider(glm::vec4 vec = glm::vec4(0, 0, 0, 0)) {//x1 y1 x2 y2
		coord = vec;
		center.x = (vec.z - vec.x)/2 + vec.x;
		center.z = (vec.w - vec.y)/2 + vec.y; 
		//vec:  1 1 5 5
		edges[0] = glm::vec4(vec.x, vec.y, vec.z, vec.y);
		edges[1] = glm::vec4(vec.x, vec.w, vec.z, vec.w);
		edges[2] = glm::vec4(vec.x, vec.y, vec.x, vec.w);
		edges[3] = glm::vec4(vec.z, vec.y, vec.z, vec.w);
	}
	int doIntersect(glm::vec2 p1, glm::vec2 q1) {

		bool xcoll = false;
		bool ycoll = false;
		for (int i = 0; i < 4; i++) {
			// Sprawdzenie czy punkty są po różnych stronach odcinków
			bool condition1 = ((p1.x - q1.x) * (edges[i].y - q1.y) > (p1.y - q1.y) * (edges[i].x - q1.x));
			bool condition2 = ((p1.x - q1.x) * (edges[i].w - q1.y) > (p1.y - q1.y) * (edges[i].z - q1.x));
			bool condition3 = ((edges[i].x - edges[i].z) * (p1.y - edges[i].w) > (edges[i].y - edges[i].w) * (p1.x - edges[i].z));
			bool condition4 = ((edges[i].x - edges[i].z) * (q1.y - edges[i].w) > (edges[i].y - edges[i].w) * (q1.x - edges[i].z));

			if ((condition1 != condition2) && (condition3 != condition4)){
				if (i < 2 == 0) {
					xcoll = true;
				}
				else
					ycoll = true;
			}
		}
		if (xcoll && ycoll)
			return 3;
		if (xcoll)
			return 1;
		if (ycoll)
			return 2;
		return 0;



	}

	int contains(float x, float y) {

		if (coord == glm::vec4(0, 0, 0, 0)) {
			return  0;
		}

		if (x > coord.x && x < coord.y && coord.z < y && coord.w >y)
		{
			return 1;
		}
		return 0;
	}

};

class CustomModel {
private:
	std::vector <glm::vec4> verts;
	std::vector <glm::vec4> norms;
	std::vector <glm::vec2> texCoords;
	std::vector <unsigned int> indices;

public:
	glm::vec3 pos;
	glm::vec2 size;
	Collider collider;
	void loadTexture(std::string filename) {
		Assimp::Importer importer;
		verts.clear();
		norms.clear();
		texCoords.clear();
		indices.clear();
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		printf(importer.GetErrorString());
		if (scene->HasMeshes()) {
			for (int i = 0; i < scene->mNumMeshes; i++) {
				auto mesh = scene->mMeshes[i];
				for (int j = 0; j < mesh->mNumVertices; j++) {

					aiVector3D vertex = mesh->mVertices[j];
					verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));
					aiVector3D normal = mesh->mNormals[j];
					norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));
					/*
					unsigned int liczba_zest = mesh->GetNumUVChannels();
					unsigned int wymiar_wsp_tex = mesh->mNumUVComponents[0];*/
					aiVector3D texCoord = mesh->mTextureCoords[0][j];
					texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
				}

				for (int j = 0; j < mesh->mNumFaces; j++) {
					auto face = mesh->mFaces[j];
					for (int k = 0; k < face.mNumIndices; k++) {
						indices.push_back(face.mIndices[k]);
					}
				}


			}

		}
	}

	void draw(glm::mat4 M, GLuint tex, glm::vec4 lp) {//TODO: verts norms and texcoords to add for multiple models
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Specify source of the data for the attribute vertex

		glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data()); //Specify source of the data for the attribute normal

		glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);

		glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
		glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoords.data()); //Specify source of the data for the attribute normal
		glUniform1i(sp->u("textureMap0"), 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tex);
		/// //////////////////////////////////////

		//glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
		glDisableVertexAttribArray(sp->a("texCoord"));
		glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
		glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
		glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
	}
};

std::vector<std::string> bottleNames = {"models/Carafe_with_stopper.obj", "models/mybottle1.obj", "models/mybottle2.obj", "models/mybottle3.obj" };
//jack working but too many meshes
CustomModel table;
CustomModel bottle;
CustomModel gigaBottle;
std::vector <CustomModel> bottleModels;//vector with bottles with diffrent models
std::vector <CustomModel> collidingModels;
std::vector <glm::vec3> bottlePositions;
std::vector <int> bottlesStillStanding;
//glm::vec3 bottlePositions[10];

bool bottleExists(int id, std::vector<int> bottlePositions) {
	auto it = std::find(bottlePositions.begin(), bottlePositions.end(), id);
	return it != bottlePositions.end();
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	srand(time(0));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0, 0, 0, 1); //Set color buffer clear color
	glEnable(GL_DEPTH_TEST); //Turn on pixel depth test based on depth buffer
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");

	skytex = readTexture("sky.png");
	woodtex = readTexture("wood.png");
	orangeGlass = readTexture("glass_orange.png");
	table.loadTexture("models/tableround.obj");
	tex0 = readTexture("wood_texture.png");
	tex1 = readTexture("floor_text.png");
	texSpecWall = readTexture("wood_specular.png");
	texSpecFloor = readTexture("wood_floor_spec.png");
	deadFloor = readTexture("dead_text.png");
	stainedGlass = readTexture("stainedGlass.png");
	
	//bottles
	for (int i = 0; i < 10; i++) {
		int selectModel = rand() % bottleNames.size();
		bottle.loadTexture(bottleNames[selectModel]);
		bottleModels.push_back(bottle);
	}
	gigaBottle.loadTexture(bottleNames[0]);
	printf("Namessize: %d BottleSize %d", bottleNames.size(), bottleModels.size());
	//Colliding models
	//glm::vec3 positions[10];
	for (int i = 0; i < 10; i++) {
		bottlesStillStanding.push_back(i);
	}
	for (int i = 0; i < 5; i++) {
		//bottlePositions[i] = glm::vec3(i * 20.0f - 50.0f, 1.0f, 21.0f);
		bottlePositions.push_back(glm::vec3(i * 20.0f - 50.0f, 1.0f, 21.0f));
	}
	for (int i = 5; i < 10; i++) {
		//bottlePositions[i] = glm::vec3((i-5) * 20.0f - 50.0f, 1.0f, -21.0f);
		bottlePositions.push_back(glm::vec3((i - 5) * 20.0f - 50.0f, 1.0f, -21.0f));
	}
	bottlePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < 10; i++) {
		table.size = glm::vec2(2.0f, 2.0f);
		table.pos = bottlePositions.at(i);
		table.collider = Collider(glm::vec4(table.pos.x - table.size.x, table.pos.z - table.size.y, table.pos.x + table.size.x, table.pos.z + table.size.y));
		collidingModels.push_back(table);
	}
}

float distanceBetweenTwoPoints(float xa, float za, float xb, float zb) {
	return sqrt(pow(xb - xa, 2) + pow(zb - za, 2));
}

int nearestBottle(std::vector<glm::vec3>& positions) {
	float radius = 10.0f;
	glm::vec3 currentBottlePosition;
	for (int i = 0; i < positions.size(); i++) {
		currentBottlePosition = positions[i];
		if (distanceBetweenTwoPoints(cameraPos.x, cameraPos.z, currentBottlePosition.x, currentBottlePosition.z) <= radius) {
			return i;
		}
	}
	return -1;
}


enum wallType{BASIC, WINDOWS, DOOR};

float sinarg = 0;


class Room {
	int height;
	float floorH;
public:
	Room(int h, float fh) {
		height = h;
		floorH = fh;
	}

	void drawPlate(glm::mat4 Mb, glm::vec3 coords, glm::vec3 cubeScal, GLuint tex, GLuint texSpec, bool reflected, float rotateAngle = 0) {
		float drunkfun = drunk_coef * sin(drunk_coef*2*sinarg) + 1.5*drunk_coef;// +drunk_coef;'
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
		Mp = glm::translate(Mb, glm::vec3(coords.x + cubeScal.x  + drunkfun , floorH + coords.y * 2 * cubeScal.y, coords.z + cubeScal.z + drunkfun));
		Mp = glm::scale(Mp, glm::vec3(cubeScal.x * scalx, cubeScal.y, cubeScal.z * scalz));
		textureCubeSpec(Mp, tex, texSpec, lpmain);
	}

	void drawWall(bool rotated, glm::mat4 M, float var, GLuint tex, GLuint texSpec, glm::vec3 plateScal, float range, bool reflected, wallType wt = BASIC) {

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
};

class Floor {
private:
	glm::vec3 plateScalNotRot = glm::vec3(1.0f, 1.0f, 0.125f);
	glm::vec3 plateScalRot = glm::vec3(0.125f, 1.0f, 1.0f);
	glm::vec3 floorScaleVec = glm::vec3(10.0f, 0.125f, 10.0f);
	int height = 5;
	int roomNum = 5;

public:
	void draw(float offset) {//offset of rooms relative to X axis
		bool rotFlag = false;
		if (offset > 0.0f) //negative offset means that rooms need to be rotated by 180 degrees around Y axis
		{
			rotFlag = true;
			offset *= -1;
		}

		for (int rn = 0; rn <= roomNum -1; rn++) { //room number
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
			wall.drawWall(1, Ms, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, rotFlag);
			wall.drawWall(1, Ms, -floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, rotFlag);

			//floor
			for (int i = 0; i < 2; i++) {
				glm::mat4 Mp = glm::translate(Ms, glm::vec3(0.0f, i * height * plateScalRot.y*2, 0.0f));//ceiling if i == 1
				Mp = glm::scale(Mp, floorScaleVec);
				textureCube(Mp, tex1, lpmain);
			}
			
			//corridor
			if (rotFlag) { //just for one side
				glm::mat4 Mc = glm::mat4(1.0f);
				Mc = glm::translate(Mc, glm::vec3((float)roomCoord, 0.0f, 0.0f));
				if (rn == 0) {
					glm::mat4 Mw = glm::translate(Mc, glm::vec3(-20.0f, 0.0f, 0.0f));
					wall.drawWall(1, Mw, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, !rotFlag, DOOR);
				}
				if (rn == roomNum-1) {
					wall.drawWall(1, Mc, floorScaleVec.z, tex0, texSpecWall, plateScalRot, floorScaleVec.z, !rotFlag, WINDOWS);
				}
				for (int i = 0; i < 2; i++) {
					Mc = glm::translate(Mc, glm::vec3(0.0f, i * height * plateScalRot.y * 2, 0.0f));//ceiling if i == 1
					Mc = glm::scale(Mc, floorScaleVec);
					textureCube(Mc,  tex1 , lpmain);
					Mc = glm::translate(glm::mat4(1.0f), glm::vec3((float)roomCoord, 0.0f, 0.0f));
				}
			}
		}

	}

};

float collisionXtab[4] = { -40.0f, -20.0f, 0.0f, 20.0f };
float precisionWall = 1.3f;
float doorWidth = 2.0f;
float doorsCoordPos[5] = { -55.0f, -35.0f, -15.0f, 5.0f, 25.0f};
float doorsCoordNeg[5] = { -45.0f, -25.0f, -5.0f, 15.0f, 35.0f };
int collisionDetected(glm::vec3 pos) {//0 - no collision, 1 - wall parallel to x axis, 2 - wall parallel to z axis, 3 - both(corner)
	bool xwall = false;
	bool zwall = false;//TODO: find better name  ( ͡° ͜ʖ ͡°)
	if (pos.z > 28.0f || pos.z < -28.0f){ //boundaries detection
		//std::cout << cameraPos.x << " " << cameraPos.z << std::endl;
		zwall = true;
	}
	if (pos.x > 38.0f || pos.x < -58.0f) {
		xwall = true;
	}
	if (pos.z > 8.5f || pos.z < -8.5f) {
		for (int i = 0; i < 4; i++) {
			if (abs(pos.x - collisionXtab[i]) < precisionWall) {
				xwall = true;
			}
		}
		if (abs(pos.z - 10.0f) < 1.3f) {
			//std::cout << cameraPos.x << " " << cameraPos.z << std::endl;
			for (int i = 0; i < 5; i++) {
				if (abs(pos.x - doorsCoordPos[i]) < doorWidth) {
					//std::cout << zwall << std::endl;
					xwall = true;
					zwall = false;
					break;
				}
				zwall = true;
			}
			
		}
		else if (abs(pos.z + 10.0f) < 1.3f) {
			//std::cout << cameraPos.x << " " << cameraPos.z << std::endl;
			for (int i = 0; i < 5; i++) {
				if (abs(pos.x - doorsCoordNeg[i]) < doorWidth) {
					xwall = true;
					zwall = false;
					break;
				}
				zwall = true;
				//xwall = true;
			}
			//xwall = false;
		}
	}
	if (xwall && zwall)
		return 3;
	else if (xwall)
		return 1;
	else if (zwall)
		return 2;
	return 0;
}


void textureCube(glm::mat4 M, GLuint tex, glm::vec4 lp, bool inside) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Specify source of the data for the attribute vertex

	glUniform1i(sp->u("negate"), inside);
	glEnableVertexAttribArray(sp->a("color")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Specify source of the data for the attribute color
	glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal


	glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, myCubeTexCoords); //Specify source of the data for the attribute normal
	glUniform1i(sp->u("textureMap0"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, tex);

	/// //////////////////////////////////////

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
}
Floor f;

void textureCubeSpec(glm::mat4 M, GLuint tex, GLuint texSpec, glm::vec4 lp, bool inside) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Specify source of the data for the attribute vertex

	glUniform1i(sp->u("negate"), inside);
	glEnableVertexAttribArray(sp->a("color")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Specify source of the data for the attribute color
	glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Specify source of the data for the attribute normal


	glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, myCubeTexCoords); //Specify source of the data for the attribute normal
	glUniform1i(sp->u("textureMap0"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUniform1i(sp->u("textureMap1"), 7);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, texSpec);



	/// //////////////////////////////////////

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
}

//Drawing procedure
void drawScene(GLFWwindow* window, float lookupAngle) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers
	tempCam = cameraPos;
	cameraPos -= glm::vec3(cameraFront.x * moveSpeedx, 0, cameraFront.z * moveSpeedx);
	cameraPos += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeedz;
	// colliders
	int colliderCheck = 0;
	int collRes = 0;
	bool collx = false;
	bool colly = false;
	collRes = collisionDetected(cameraPos);
	if (collRes == 3) {
		collx = true;
		colly = true;
	}
	else if (collRes == 1) {
		collx = true;
	}
	else if (collRes == 2) {
		colly = true;
	}
	for (int i = 0; i < collidingModels.size(); i++) {
		colliderCheck = collidingModels.at(i).collider.doIntersect(glm::vec2(tempCam.x, tempCam.z), glm::vec2(cameraPos.x, cameraPos.z)); // -1.0f to compense a shift between hitbox and texture
		if (colliderCheck == 3) {
			collx = true;
			colly = true;
		}
		else if (colliderCheck == 1) {//TO FIX
			collx = true;
		}
		else if (colliderCheck == 2) {
			colly = true;
		}
	}
	//
	if (collx && colly) {
		cameraPos.x = tempCam.x;
		cameraPos.z = tempCam.z;
		std::cout << "3" << std::endl;
	}
	else if (collx) {
		cameraPos.x = tempCam.x;
		std::cout << "1" << std::endl;
	}
	else if (colly) {
		cameraPos.z = tempCam.z;
		//std::cout << "2" << std::endl;
	}
	glm::vec3 cameraDir = glm::vec3(cameraPos.x + cameraFront.x, cameraFront.y, cameraPos.z + cameraFront.z); // ????
	glm::mat4 P = glm::perspective(glm::radians(50.0f + sin(sinarg * 2 * drunk_coef)*drunk_coef*20), 1.0f, 1.0f, 150.0f); //Compute projection matrix
	glm::mat4 V = glm::lookAt(cameraPos,
		cameraPos + cameraFront + glm::vec3(0.0f + sin(sinarg* 2*drunk_coef) * drunk_coef * 1/2, lookupAngle + cos(sinarg * 2 * drunk_coef) * drunk_coef * 1/2, 0.0f + sin(sinarg * 2 * drunk_coef) * drunk_coef * 1/2),
		glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
	//V = glm::rotate(V, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	
	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));


	//modelPos.clear();

	
	f.draw(10.0f);
	f.draw(-20.0f); 

	glm::mat4 M = glm::mat4(1.0f);
	for (int i = 0; i < collidingModels.size(); i++) {
		
		//tables drawing
		M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3(collidingModels.at(i).pos.x - 1.0f, collidingModels.at(i).pos.y, collidingModels.at(i).pos.z - 1.0f));
		collidingModels.at(i).draw(M, woodtex, lpmain);
		if (!bottleExists(i, bottlesStillStanding)) continue;
		
		//bottles drawing
		M = glm::translate(M, glm::vec3(1.0f, 1.7f, 1.0f));
		M = glm::scale(M, glm::vec3(5.0f, 5.0f, 5.0f));
		bottleModels[i].draw(M, orangeGlass, lpmain);
	}

	
	//Skybox
	M = glm::mat4(1.0f);
	M = glm::scale(M, glm::vec3(80.0f, 100.0f, 70.0f));
	textureCube(M, skytex, glm::vec4(0.0f, 300.0f, 0.0f, 1.0f), true);

	M = glm::mat4(1.0f);
	M = glm::translate(M, cameraPos);

	if (drunkLevel == 10) {
		glm::mat4 M = glm::mat4(1.0f);
		M = glm::translate(M, glm::vec3(1.0f, 5.0f, 1.0f));
		M = glm::scale(M, glm::vec3(20.0f, 20.0f, 20.0f));
		gigaBottle.draw(M, stainedGlass, lpmain);
	}

	glfwSwapBuffers(window); //Copy back buffer to the front buffer
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(900, 900, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it. 

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure

	//SFML
	sf::SoundBuffer buffer1, buffer2, buffer3, buffer4, buffer5, buffer6;
	if (!buffer1.loadFromFile("entertainer.wav") 
		|| !buffer2.loadFromFile("drink.wav")
		|| !buffer3.loadFromFile("entertainer125.wav")
		|| !buffer4.loadFromFile("entertainer150.wav")
		|| !buffer5.loadFromFile("entertainer175.wav")
		|| !buffer6.loadFromFile("lacrymosa.wav")) {
		return 1; // Error loading sound files
	}

	sf::Sound backgroundMusic, drinkingSound;
	backgroundMusic.setBuffer(buffer1);
	drinkingSound.setBuffer(buffer2);

	backgroundMusic.play();

	//Main application loop

	float drunkIncrement = 0.2f;

	float music125toggleTime = 0;
	float music150toggleTime = 0;
	
	float startAngle = 0.0f;
	float musicPitch = 1;
	float musicVolume = 20;
	bool ascending = false;
	backgroundMusic.setVolume(musicVolume);
	sf::Time startOffset;
	glfwSetTime(0); //clear internal timer
	while (!glfwWindowShouldClose(window) && !close) //As long as the window shouldnt be closed yet...
	{
		sinarg = glfwGetTime();
		
		std::cout << sinarg << std::endl;
		//glfwSetTime(0); //clear internal timer
		if (!ascending) startAngle = 0.0f;
		drawScene(window, startAngle); //Execute drawing procedure
		int nearestBottleId = nearestBottle(bottlePositions);
		if (drinkUp && nearestBottleId != -1 && bottleExists(nearestBottleId, bottlesStillStanding)) {
			bottlesStillStanding.erase(std::remove(bottlesStillStanding.begin(), bottlesStillStanding.end(), nearestBottleId), bottlesStillStanding.end());
			moveSpeedx = 0;
			moveSpeedz = 0;
			double timeToStop = glfwGetTime() + 1.2f;
			drinkingSound.play();
			while (glfwGetTime() < timeToStop) {
				drawScene(window, startAngle);
				startAngle += 0.02f;
			}
			drinkUp = false;
			drunkLevel += 1;
			drunk_coef = drunkIncrement * drunkLevel;
			if (drunkLevel == 3) {
				backgroundMusic.setBuffer(buffer3);
				startOffset = sf::seconds(glfwGetTime() * 100.0 / 125.0);
				music125toggleTime = glfwGetTime();
				//glfwSetTime(glfwGetTime() * 100.0 / 125.0);
				backgroundMusic.setPlayingOffset(startOffset);
				backgroundMusic.play();
			}
			if (drunkLevel == 6) {
				backgroundMusic.setBuffer(buffer4);
				float normalSongTime = music125toggleTime + (glfwGetTime() - music125toggleTime) * 125 / 100;
				startOffset = sf::seconds(normalSongTime * 100.0 / 150.0);
				music150toggleTime = glfwGetTime();
				backgroundMusic.setPlayingOffset(startOffset);
				backgroundMusic.play();
			}
			if (drunkLevel == 9) {
				backgroundMusic.setBuffer(buffer5);
				float normalSongTime = music125toggleTime  + (music150toggleTime - music125toggleTime) * 125 / 100 + (glfwGetTime() - music150toggleTime) * 150 / 100;
				startOffset = sf::seconds(normalSongTime * 100.0 / 175.0);
				backgroundMusic.setPlayingOffset(startOffset);
				backgroundMusic.play();
			}
			if (drunkLevel == 1) {
				drunk_coef = 0.0;
				backgroundMusic.setBuffer(buffer6);
				backgroundMusic.play();
				defaultSpeed /= 4;
			}
		}

		if (drinkUp && nearestBottle(bottlePositions) == 10 && drunkLevel == 10) {
			/*moveSpeedx = 0;
			moveSpeedz = 0;*/
			double timeToStop = glfwGetTime() + 5.0f;
			drinkingSound.play();
			while (glfwGetTime() < timeToStop) {
				drawScene(window, startAngle);
				startAngle += 0.02f;
			}
			timeToStop = glfwGetTime() + 5.0f;
			ascending = true;
			while (glfwGetTime() < timeToStop) {
				cameraPos += glm::vec3(0, 0.01f, 0);
				drawScene(window, startAngle);
			}
			drinkUp = false;
			moveSpeedx = -0.08f;
		}

		std::cout << nearestBottle(bottlePositions) << std::endl;
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
 
