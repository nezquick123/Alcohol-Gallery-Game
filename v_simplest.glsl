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
//Varying variables
out vec4 ic;
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;
void main(void) {
    int normal_inside_coeff = 1; // -1 when skybox is textured to negate normals
    if(negate == 1)
        normal_inside_coeff *=-1;
    l = normalize(V * lp - V * M * vertex ); //vector towards the light in eye space
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //vector towards the viewer in eye space
    n = normalize(V * M * normal* normal_inside_coeff); //normal vector in eye spaces
    ic = color;
    iTexCoord0 = texCoord;
    gl_Position = P * V * M * vertex;
}
