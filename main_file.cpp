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

float horizCamDirectSpeed = 0;//[radians/s]
float vertCamDirectSpeed = 0; 
glm::vec3 cameraPos = glm::vec3(0.0f, 7.0f, 9.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
float moveSpeedx = 0;
float moveSpeedz = 0;
ShaderProgram* sp; //Pointer to the shader program
GLuint tex0;
GLuint tex1;


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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

float* vertices = myCubeVertices;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
float* normals = myCubeNormals;
int vertexCount = myCubeVertexCount;

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}




//Procedura obs³ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) horizCamDirectSpeed = -PI; //Je¿eli wciœniêto klawisz "w lewo" ustaw prêdkoœæ na -PI
		if (key == GLFW_KEY_RIGHT) horizCamDirectSpeed = PI; //Je¿eli wciœniêto klawisz "w prawo" ustaw prêdkoœæ na PI
		if (key == GLFW_KEY_UP) vertCamDirectSpeed = PI; //Je¿eli wciœniêto klawisz "w gore" ustaw prêdkoœæ na PI
		if (key == GLFW_KEY_DOWN) vertCamDirectSpeed = -PI; //Je¿eli wciœniêto klawisz "w dol" ustaw prêdkoœæ na -PI
		if (key == GLFW_KEY_W) moveSpeedx = -0.5f;
		if (key == GLFW_KEY_S) moveSpeedx = 0.5f;
		if (key == GLFW_KEY_A) moveSpeedz = -0.5f;
		if (key == GLFW_KEY_D) moveSpeedz = 0.5f;
		
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) horizCamDirectSpeed = 0;
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) vertCamDirectSpeed = 0;
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
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	delete sp;
	//************Place any code here that needs to be executed once, after the main loop ends************
}


void textureModel(glm::mat4 M, GLuint tex) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Specify source of the data for the attribute vertex

	glEnableVertexAttribArray(sp->a("color")); //Enable sending data to the attribute color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Specify source of the data for the attribute color

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

void drawPlate(glm::mat4 Mb, float x, float y, float z, glm::vec3 floorScale, glm::vec3 cubeScal, GLuint tex, float rotateAngle=0) {

	glm::mat4 Mp = glm::rotate(Mb, rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	Mp = glm::translate(Mb, glm::vec3(x - cubeScal.x + 2 * cubeScal.x, floorScale.y + y* 2 * cubeScal.y, z - cubeScal.z + 2 * cubeScal.z));
	Mp = glm::scale(Mp, cubeScal);
	textureModel(Mp, tex);
	
}

void drawRoom(float offset) {
	bool rotFlag = false;
	if (offset > 0.0f) //negative offset means that rooms need to be rotated by 180 degrees around Y axis
	{
		rotFlag = true;
		offset *= -1;
	}
	glm::vec3 wallScalx = glm::vec3(1.0f, 1.0f, 0.125f);
	glm::vec3 wallScalz = glm::vec3(0.125f, 1.0f, 1.0f);
	//Cube
	glm::vec3 floorScaleVec = glm::vec3(10.0f, 0.125f, 10.0f);

	for (int i = -50; i < 50; i += 20) {

		glm::mat4 Ms = glm::mat4(1.0f);
		if (rotFlag) //rotate and correct x offset
		{
			Ms = glm::rotate(Ms, PI, glm::vec3(0.0f, 1.0f, 0.0f));
			Ms = glm::translate(Ms, glm::vec3(20.0f, 0.0f, offset));
		}
		
		Ms = glm::translate(Ms, glm::vec3((float)i, 0.0f, offset));
		

		//draw surrounding walls
		int height = 5;
		int j = 0;
		for (float h = 0; h <= height; h += 1) {

			for (float i = -floorScaleVec.x, j = 0; i < floorScaleVec.x; i += 2 * wallScalx.x, j++) {
				if (!(h >= 0 && h < 5 && j >= 6 && j <= 8))
					drawPlate(Ms, i, h, floorScaleVec.z, floorScaleVec, wallScalx, tex0);
				if (!(h >= 2 && h <= 4 && ((j >= 1 && j <= 3) || (j >= 6 && j <= 8))))// okna
					drawPlate(Ms, i, h, -floorScaleVec.z, floorScaleVec, wallScalx, tex0);

			}
			for (float i = -floorScaleVec.z, j = 0; i < floorScaleVec.z; i += 2 * wallScalz.z, j++) {
				drawPlate(Ms, floorScaleVec.x, h, i, floorScaleVec, wallScalz, tex0, 90);
				drawPlate(Ms, -floorScaleVec.x, h, i, floorScaleVec, wallScalz, tex0, 90);
			}
		}

		//floor
		glm::mat4 Mp = glm::scale(Ms, floorScaleVec);
		textureModel(Mp, tex1);
	}
	//corridor
	for (int i = -50; i < 50; i += 20) {

		glm::mat4 Ms = glm::mat4(1.0f);
		Ms = glm::translate(Ms, glm::vec3((float)i, 0.0f, 0.0f));
		Ms = glm::scale(Ms, floorScaleVec);
		textureModel(Ms, tex1);
	}
}


//Drawing procedure
void drawScene(GLFWwindow* window, float angle, float vertAngle) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers
	
	cameraPos -= glm::vec3(cameraFront.x * moveSpeedx, 0, cameraFront.z * moveSpeedx);
	cameraPos += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeedz;

	glm::vec3 cameraDir = glm::vec3(cameraPos.x + cameraFront.x, cameraFront.y, cameraPos.z + cameraFront.z);
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 150.0f); //Compute projection matrix
	glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix

 
	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	//M = glm::translate(M, cameraPos);
	drawRoom(10.0f);
	drawRoom(-20.0f);
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
	float horizAngle = 0; //declare variable for storing current rotation angle
	float vertAngle = 0;
	glfwSetTime(0); //clear internal timer
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		horizAngle += horizCamDirectSpeed * glfwGetTime(); //Compute an angle by which the object was rotated during the previous frame
		vertAngle += vertCamDirectSpeed * glfwGetTime(); //Compute an angle by which the object was rotated during the previous frame
		glfwSetTime(0); //clear internal timer
		drawScene(window, horizAngle, vertAngle); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
 