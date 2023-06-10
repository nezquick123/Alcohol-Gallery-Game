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
uniform sampler2D textureMap2;


vec2 parallaxTexCoords(vec4 v, vec2 t, float h, float s){
	vec2 ti = -v.xy/s;
	float hi = -v.z/s;
	vec2 tc=t;
	float hc=h;
	float ht=h*texture(textureMap2,tc).r;

	if(v.z<=0)discard;
	while(hc>ht){
		tc=tc+ti;
		if(tc.x<0||tc.x>1||tc.y<0||tc.y>1)discard;
		hc=hc+hi;
		ht=h*texture(textureMap2,tc).r;
	}
	return tc;
}

void main(void) {
    //Znormalizowane interpolowane wektory
	float brightness = 0.5;
	vec4 ml = normalize(l);

	vec4 mv=normalize(v);
	vec2 nt=parallaxTexCoords(mv,iTexCoord0,0.1,100);

	vec4 mn = normalize(vec4(texture(textureMap1, nt).rgb*2-1,0));
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);

	//Parametry powierzchni
	vec4 kd = texture(textureMap0, nt); 
	//vec4 ks = texture(textureMap1, nt);
	vec4 ks = vec4(1,1,1,1);

	// Adjust the brightness by multiplying the kd and ks values with the brightness factor
    kd.rgb *= brightness;
    ks.rgb *= brightness;

	//Obliczenie modelu o�wietlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1),25);
	pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);
}