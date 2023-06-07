#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp;
uniform int negate;
//Attributes
in vec4 vertex; //Vertex coordinates in model space
in vec4 color; //vertex color
in vec4 normal; //Vertex normal in model space
in vec2 texCoord;

in vec4 c1;
in vec4 c2;
in vec4 c3;

//Varying variables
out vec4 ic;
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;
void main(void) {
    mat4 invTBN = mat4(c1,c2,c3,vec4(0,0,0,1));
    
	l = normalize(invTBN*(inverse(M) * lp - vertex)); //wektor do œwiat³a w przestrzeni oka
    v = normalize(invTBN*(inverse(V*M)*vec4(0, 0, 0, 1) - vertex)); //wektor do obserwatora w przestrzeni oka
    //n = normalize(V * M * normal); //wektor normalny w przestrzeni oka
    iTexCoord0 = texCoord;
    

    //ic = color;
    
    gl_Position=P*V*M*vertex;
}
