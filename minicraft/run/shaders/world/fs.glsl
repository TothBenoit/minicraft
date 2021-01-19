#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;

out vec4 color_out;

//Globales
uniform float att = 0.4; //attenuation 

void main()
{
	color_out = vec4(sqrt(color.rgb*att),color.a);
}