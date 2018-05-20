#version 330

uniform float in_SightRange;

in vec3 fs_Color;
in vec3 fs_Normal;
in vec3 fs_Position;

void main()
{
	gl_FragData[0] = vec4(fs_Color, 1.0);
	gl_FragData[1] = vec4((normalize(fs_Normal) + 1.0) / 2.0, clamp(fs_Position.z / -in_SightRange, 0.0, 1.0));
}