#version 400

in vec2 uv;

uniform mat4 v;
uniform mat4 p;
uniform vec3 sunPos;
uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;
uniform vec2 near_far;
uniform float exposure = 1;

int NUM_SAMPLES = 64;
float Density = 0.97f;
float Weight = 0.05f;
float Decay = .974f;
float exposureValue = 0.24f;


out vec4 color_out;

float LinearizeDepth(float z)
{
	float n = near_far.x; // camera z near
  	float f = near_far.y; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D(TexColor, uv);
	float depth = texture2D(TexDepth, uv).r;	
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);

	color.r = pow(color.r,1.0/2.2);
	color.g = pow(color.g,1.0/2.2);
	color.b = pow(color.b,1.0/2.2);

	vec2 texCoord = uv;
	vec4 screenLightPos = vec4(sunPos.xyz, 1);
	screenLightPos = v * screenLightPos;

	float zSun = normalize(screenLightPos).z;
	zSun = (zSun + 1)/2;
	//insuffisant quand le soleil est hors caméra
	screenLightPos = p * screenLightPos; 
	screenLightPos /= screenLightPos.w;
	screenLightPos.x = clamp((screenLightPos.x + 1)/2,0,1);
	screenLightPos.y = clamp((screenLightPos.y + 1)/2,0,1);

	vec2 deltaTexCoord = uv - screenLightPos.xy;
	deltaTexCoord *= 1.0f/(NUM_SAMPLES * Density);

	float godRay = texture(TexColor, uv).x;

	float illuminationDecay = 1.0f;
	for (int i  = 0; i < NUM_SAMPLES; i++)
	{
		texCoord -= deltaTexCoord;
		float Sample = texture(TexColor, texCoord).x;
		Sample *= illuminationDecay * Weight;
		godRay += Sample;
		illuminationDecay *= Decay;
	}

	color_out = vec4(color.rgb + godRay * mix(mix(0,exposureValue,exposure),0,zSun) ,color.a);

}
/*
float4 main(float2 texCoord : TEXCOORD0) : COLOR0 {   
	// Calculate vector from pixel to light source in screen space.    
	half2 deltaTexCoord = (texCoord - ScreenLightPos.xy);   
	// Divide by number of samples and scale by control factor.   
	deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;  
	// Store initial sample.    
	half3 color = tex2D(frameSampler, texCoord);   
	// Set up illumination decay factor.    
	half illuminationDecay = 1.0f;   
	// Evaluate summation from Equation 3 NUM_SAMPLES iterations.    
	for (int i = 0; i < NUM_SAMPLES; i++)   {     
		// Step sample location along ray.     
		texCoord -= deltaTexCoord;     
		// Retrieve sample at new location.    
		half3 sample = tex2D(frameSampler, texCoord);     
		// Apply sample attenuation scale/decay factors.     
		sample *= illuminationDecay * Weight;     
		// Accumulate combined color.     
		color += sample;
		// Update exponential decay factor.     
		illuminationDecay *= Decay;   
	}   
	// Output final color with a further scale control factor.    
	return float4( color * Exposure, 1); 
} 
*/