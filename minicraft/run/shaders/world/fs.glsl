#version 400

//Variables en entree
in vec3 wPosOut;
in vec3 normal;
in vec4 color;
in vec2 uv;

out vec4 color_out;

//Globales
uniform sampler2D atlas;
uniform float elapsed;
uniform vec3 skyColor; 
uniform vec3 sunPos;
uniform float ambientLevel = 0.1f;
uniform vec3 specularColor = vec3(1,1,1);
uniform vec3 camPos;
uniform float night;

float glossiness = 0.05f;
float delta = 0.2f;

void Noise(inout vec3 pos)
{
	float zNoise;
	pos.z -=1;
	zNoise += sin(pos.x/20+elapsed)*0.75f;
	zNoise += sin((pos.x+pos.y)/2 + elapsed*1.7f)*0.15f;
	zNoise += sin((pos.x+2*pos.y)/2 + elapsed*2.75f)*0.05f;
	pos.z += zNoise;
}
void main()
{
	vec3 niceNormal = normal;
	vec4 texColor = texture(atlas, uv);

	//Only water has alpha < 1
	//We recalculate normal for water
	if(color.a < 0.9)
	{
		glossiness = 2f;
		vec3 p1 = wPosOut;
		vec3 p2 = wPosOut + vec3(0.02,0,0);
		vec3 p3 = wPosOut + vec3(0,0.02,0);
		Noise(p1);
		Noise(p2);
		Noise(p3);
		vec3 v1 = p2-p1;
		vec3 v2 = p3-p1;
		niceNormal = normalize(cross(v1,v2));
	}

	float backIllumination = 0;
	// si nos uv sont en dehors de la texture il s'agit d'eau ou de nuage
	if(uv.x + uv.y > 1.9f)
	{
		texColor = color;
	}

	//Calculate SSS for clouds (clouds is white and the only one with r > 0.99)
	if (color.r >0.99)
	{
		vec3 sunToObjDir = normalize(-sunPos);
		vec3 camToObjDir = normalize(wPosOut-camPos);

		backIllumination = clamp(dot(camToObjDir, - normalize(sunToObjDir +(-camToObjDir * delta))),0,1);
		backIllumination = pow(backIllumination,8)*0.5f;
	}

	vec3 sunDir = normalize(sunPos);
	texColor.r = pow(texColor.r, 2.2);
	texColor.g = pow(texColor.g, 2.2);
	texColor.b = pow(texColor.b, 2.2);



	float lumiAmbient = skyColor.x*0.05f + skyColor.y * 0.3f + skyColor.z * 0.15f;

	float diffuse = max(dot(niceNormal,sunDir),0.01f + lumiAmbient);
	
	vec3 ambientColor = clamp((1-diffuse),0,1) * skyColor * ambientLevel;

	vec3 halfV = normalize(sunDir + normalize(camPos-wPosOut));
	float specularScalar =  pow(max(dot(halfV, niceNormal),0),250);
	
	color_out = vec4((texColor.rgb + specularColor * specularScalar * glossiness)* diffuse + ambientColor + specularColor * backIllumination * (1-night) ,texColor.a);
}