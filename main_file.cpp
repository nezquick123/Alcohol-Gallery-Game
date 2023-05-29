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
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <scene.h>
#include <iostream>
#include <vector>
#include <math.h>
void textureCube(glm::mat4 M, GLuint tex, glm::vec4 lp, bool inside=false);

GLuint tex0;
GLuint tex1;
GLuint skytex;
glm::vec3 cameraPos = glm::vec3(0.0f, 7.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 tempCam = cameraPos;
float moveSpeedx = 0;
float moveSpeedz = 0;
ShaderProgram* sp; //Pointer to the shader program
glm::vec4 lpmain = glm::vec4(0, 9, 0, 1); //light position, world space


std::vector<glm::vec4> verts;
std::vector<glm::vec4> norms;
std::vector<glm::vec2> texCoordsv2;
std::vector<unsigned int> indices;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 900.0f / 2.0;
float lastY = 900.0f / 2.0;
float fov = 45.0f;

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
void loadModel(std::string filename){
	Assimp::Importer importer;
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
				unsigned int wymiar_wsp_tex = mesh->mNumUVComponents[0]; */
				aiVector3D texCoord = mesh->mTextureCoords[0][j];
				texCoordsv2.push_back(glm::vec2(texCoord.x, texCoord.y));
			}

			for (int j = 0; j < mesh->mNumFaces; j++) {
				auto face = mesh->mFaces[j];
				for (int k = 0; k < face.mNumIndices; k++) {
					indices.push_back(face.mIndices[k]);
				}
			}
			
			if (scene->HasMaterials()) {
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				for (int j = 0; j < 19; j++) {
					std::cout << j << " " << material->GetTextureCount((aiTextureType)j) << std::endl;
				}
			}
			
		}
	}
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




//Procedura obsługi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) moveSpeedx = -0.8f;
		if (key == GLFW_KEY_S) moveSpeedx = 0.8f;
		if (key == GLFW_KEY_A) moveSpeedz = -0.8f;
		if (key == GLFW_KEY_D) moveSpeedz = 0.8f;
		if (key == GLFW_KEY_ESCAPE) close = true;
		
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_S || key == GLFW_KEY_W) moveSpeedx = 0;
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) moveSpeedz = 0;
	}
}




//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0, 0, 0, 1); //Set color buffer clear color
	glEnable(GL_DEPTH_TEST); //Turn on pixel depth test based on depth buffer
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex0 = readTexture("wall_1.png");
	tex1 = readTexture("floor_text.png");
	skytex = readTexture("sky.png");
	//loadModel("models/objBeer.obj");
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	delete sp;
	//************Place any code here that needs to be executed once, after the main loop ends************
}

enum wallType{BASIC, WINDOWS, DOOR};

float sinarg = 0;
float drunk_coef = 0.0f;

class Room {
	int height;
	float floorH;
public:
	Room(int h, float fh) {
		height = h;
		floorH = fh;
	}

	void drawPlate(glm::mat4 Mb, glm::vec3 coords, glm::vec3 cubeScal, GLuint tex, bool reflected, float rotateAngle = 0) {
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
		textureCube(Mp, tex, lpmain);
	}

	void drawWall(bool rotated, glm::mat4 M, float var, GLuint tex, glm::vec3 plateScal, float range, bool reflected, wallType wt = BASIC) {

		glm::vec3 plateCoords;
		int min, max;
		int j = 0;
		if (rotated) {//z axis
			for (float h = 0; h <= height; h += 1) {
				for (float i = -range, j = 0; i < range; i += 2 * plateScal.z, j++) {
					if (wt == DOOR) {
						if (!(h >= 0 && h < 5 && j >= 6 && j <= 8))
							drawPlate(M, glm::vec3(var, h, i), plateScal, tex, reflected, 90);
					}
					else if (wt == WINDOWS) {
						if (!(h >= 2 && h <= 4 && ((j >= 1 && j <= 3) || (j >= 6 && j <= 8))))
							drawPlate(M, glm::vec3(var, h, i), plateScal, tex, reflected, 90);
					}
					else
						drawPlate(M, glm::vec3(var, h, i), plateScal, tex, reflected, 90);
				}
			}
		}
		else {
			for (float h = 0; h <= height; h += 1) {
				for (float i = -range, j = 0; i < range; i += 2 * plateScal.x, j++) {
					if (wt == DOOR) {
						if (!(h >= 0 && h < 5 && j >= 6 && j <= 8))
							drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, reflected);
					}
					else if (wt == WINDOWS) {
						if (!(h >= 2 && h <= 4 && ((j >= 1 && j <= 3) || (j >= 6 && j <= 8))))
							drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, reflected);
					}
					else 
						drawPlate(M, glm::vec3(i, h, var), plateScal, tex0, reflected);
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
			wall.drawWall(0, Ms, floorScaleVec.x, tex0, plateScalNotRot, floorScaleVec.x, rotFlag, DOOR);
			wall.drawWall(0, Ms, -floorScaleVec.x, tex0, plateScalNotRot, floorScaleVec.x, rotFlag, WINDOWS);
			wall.drawWall(1, Ms, floorScaleVec.z, tex0, plateScalRot, floorScaleVec.z, rotFlag);
			wall.drawWall(1, Ms, -floorScaleVec.z, tex0, plateScalRot, floorScaleVec.z, rotFlag);

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
					wall.drawWall(1, Mw, floorScaleVec.z, tex0, plateScalRot, floorScaleVec.z, !rotFlag, DOOR);
				}
				if (rn == roomNum-1) {
					wall.drawWall(1, Mc, floorScaleVec.z, tex0, plateScalRot, floorScaleVec.z, !rotFlag, WINDOWS);
				}
				for (int i = 0; i < 2; i++) {
					Mc = glm::translate(Mc, glm::vec3(0.0f, i * height * plateScalRot.y * 2, 0.0f));//ceiling if i == 1
					Mc = glm::scale(Mc, floorScaleVec);
					textureCube(Mc, tex1, lpmain);
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

void textureCustom(glm::mat4 M, GLuint tex, glm::vec4 lp) {//TODO: verts norms and texcoords to add for multiple models
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data()); //Specify source of the data for the attribute normal

	glUniform4f(sp->u("lp"), lp.x, lp.y, lp.z, 1);

	glEnableVertexAttribArray(sp->a("texCoord")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsv2.data()); //Specify source of the data for the attribute normal
	glUniform1i(sp->u("textureMap0"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, tex);
	/// //////////////////////////////////////

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Draw the object
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("color")); //Disable sending data to the attribute color
	glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
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

//Drawing procedure
void drawScene(GLFWwindow* window) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers
	tempCam = cameraPos;
	cameraPos -= glm::vec3(cameraFront.x * moveSpeedx, 0, cameraFront.z * moveSpeedx);
	cameraPos += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeedz;
	
	if (collisionDetected(cameraPos) == 3) {
		cameraPos.x = tempCam.x;
		cameraPos.z = tempCam.z;
		std::cout << "3" << std::endl;
	}
	else if (collisionDetected(cameraPos) == 1) {
		cameraPos.x = tempCam.x;
		//std::cout << "1" << std::endl;
	}
	else if (collisionDetected(cameraPos) == 2) {
		cameraPos.z = tempCam.z;
		//std::cout << "2" << std::endl;
	}
	glm::vec3 cameraDir = glm::vec3(cameraPos.x + cameraFront.x, cameraFront.y, cameraPos.z + cameraFront.z);
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 150.0f); //Compute projection matrix
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix

 
	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	//M = glm::rotate(M, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//textureCustom(M, tex1);

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(100.0f, 100.0f, 100.0f));
	textureCube(M, skytex, glm::vec4(0.0f, 360.0f, 0.0f, 1.0f), true);

	M = glm::mat4(1.0f);
	M = glm::translate(M, cameraPos);
	
	f.draw(10.0f);
	f.draw(-20.0f); 



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

	//Main application loop

	glfwSetTime(0); //clear internal timer
	while (!glfwWindowShouldClose(window) && !close) //As long as the window shouldnt be closed yet...
	{
		sinarg += glfwGetTime();
		glfwSetTime(0); //clear internal timer
		drawScene(window); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
 
