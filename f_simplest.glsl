#version 330

out vec4 pixelColor; //Output variable. Almost final pixel color.

//Varying variables
in vec4 ic;
in vec4 n;
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;
in vec2 iTexCoord2;
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform int negate;
//uniform sampler2D textureMap2;
void main(void) {
    //Normalized, interpolated vectors
    vec4 ml = normalize(l);
    vec4 mn = normalize(n);
    vec4 mv = normalize(v);
    //Reflected vector
    vec4 mr = reflect(-ml, mn);

    //Surface parameters

    // Lawful
    vec4 ks = texture(textureMap1, iTexCoord0);
    vec4 kd = texture(textureMap0, iTexCoord0);

    // Chaotic
    //vec4 kd = mix(texture(textureMap0, iTexCoord0), texture(textureMap1, iTexCoord1),0);
    //vec4 ks = vec4(1,1,1,1);

    // Chaotic better
    //vec4 kd = mix(texture(textureMap0, iTexCoord0), texture(textureMap2, iTexCoord2),0.5);
    //vec4 ks = texture(textureMap1, iTexCoord0);
    float brightness = 0.6;
    if(negate == 1){
    kd.rgb *= brightness;
    kd.rgb *= brightness;
    }
    //Lighting model computation
    float nl = clamp(dot(mn, ml), 0, 1);
    float rv = pow(clamp(dot(mr, mv), 0, 1), 50);
    pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv, 0);
}