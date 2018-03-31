#version 330

in vec3 fs_Color;
in vec3 fs_Normal;

const vec3 lightDirection = normalize(vec3(0.5, 1.0, 0.0));

void main()
{
	float diffuseFactor = max(0.0, dot(normalize(fs_Normal), lightDirection));
	
	if(diffuseFactor >= 0.8)
	{
		diffuseFactor = 0.9;
	}
	else if(diffuseFactor  >= 0.7)
	{
		diffuseFactor = 0.75;
	}
	else if(diffuseFactor  >= 0.5)
	{
		diffuseFactor = 0.6;
	}
	else if(diffuseFactor  >= 0.3)
	{
		diffuseFactor = 0.4;
	}
	else if(diffuseFactor >= 0.2)
	{
		diffuseFactor = 0.25;
	}
	else
	{
		diffuseFactor = 0.0;
	}
	
	gl_FragColor = vec4(fs_Color * diffuseFactor, 1.0);

}