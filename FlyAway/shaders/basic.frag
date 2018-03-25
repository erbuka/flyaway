#version 330

in vec3 fs_Color;
in vec3 fs_Normal;

const vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));

void main()
{
	float diffuseFactor = max(0.0, dot(normalize(fs_Normal), lightDirection));
	gl_FragColor = vec4(fs_Color * diffuseFactor, 1.0);
}