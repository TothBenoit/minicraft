#version 400

//Variables en entree
in vec3 wPosOut;
in vec3 normal;
in vec4 color;
in vec2 uv;

out vec4 color_out;

//Globales
uniform float elapsed;
uniform vec3 ambientColor; 
uniform vec3 sunPos;
uniform float ambientLevel = 0.1f;
uniform vec3 specularColor = vec3(1,1,1);
uniform vec3 camPos;

float glossiness = 0.5f;

void Noise(inout vec3 pos)
{
	float zNoise;
	pos.z -=1;
	zNoise += sin(pos.x/20+elapsed)*0.2f;
	zNoise += sin((pos.x+pos.y)/2 + elapsed*1.7f)*0.1f;
	zNoise += sin((pos.x+2*pos.y)/2 + elapsed*2.75f)*0.05f;
	pos.z += zNoise;
}
void main()
{
	vec3 niceNormal = normal;
	if(color.a < 0.9)
	{
		glossiness = 20f;
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

	vec3 sunDir = normalize(sunPos);

	float lumiAmbient = ambientColor.x*0.1f + ambientColor.y * 0.6f + ambientColor.z * 0.3f;

	float diffuse = max(dot(niceNormal,sunDir),0.03f + lumiAmbient);
	
	vec3 ambient = (1-diffuse) * ambientColor * ambientLevel;

	vec3 halfV = normalize(sunDir + normalize(camPos-wPosOut));
	float specularScalar =  pow(max(dot(halfV, niceNormal),0),400);
	
	color_out = vec4((color.rgb + specularColor * specularScalar * glossiness)* diffuse + ambient ,color.a);
}