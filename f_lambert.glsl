#version 330


out vec4 pixelColor; //Fragment shader output variable

//Varying variables
in vec4 i_color;

void main(void) {
	pixelColor=i_color;
}
