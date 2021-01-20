#version 400

uniform float elapsed;
uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 iv;
uniform mat4 nmat;

layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in vec4 vs_color_in;

//Variables en sortie
out vec3 wPosOut;
out vec3 normal;
out vec4 color;
out vec2 uv;

void main()
{
	
	vec4 vecIn = vec4(vs_position_in,1.0);
	vec4 oPos = vecIn;
	vec4 wPos = m * oPos;
	wPosOut = wPos.xyz;

	float zNoise = 0;
	vec4 color_in = vs_color_in;
	if (vs_color_in.a == 0.8)
	{
		wPos.z -=1;
		zNoise += sin(wPos.x/20+elapsed)*0.5f;
		zNoise += sin((wPos.x+wPos.y)/2 + elapsed*1.7f)*0.1f;
		zNoise += sin((wPos.x+2*wPos.y)/2 + elapsed*2.75f)*0.05f;
		wPos.z += zNoise;
	}

	vec4 vPos = v * wPos; 

	gl_Position = p * vPos;
		
	normal = normalize((nmat * vec4(vs_normal_in,1.0)).xyz); 

	uv = vs_uv_in/vec2(32,1);

	color = color_in;
}