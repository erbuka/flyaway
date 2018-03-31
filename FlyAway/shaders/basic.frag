#version 330

in vec3 fs_Color;
in vec3 fs_Normal;
in vec3 fs_Position;

const vec3 lightDirection = normalize(vec3(0.5, 1.0, 0.0));

const float levels[4] = float[4]( 0.0, 0.25, 0.5, 0.75 );

void main()
{
	float diffuseFactor = dot(fs_Normal, normalize(-fs_Position)) < 0.1 ?
		0.0 : max(0.0, dot(normalize(fs_Normal), lightDirection));		
	gl_FragColor = vec4(fs_Color * diffuseFactor, 1.0);

}