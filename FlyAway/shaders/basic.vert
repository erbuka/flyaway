#version 330

uniform mat4 in_ProjectionMatrix;
uniform mat4 in_ModelViewMatrix;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Color;

smooth out vec3 fs_Color;
smooth out vec3 fs_Normal;
smooth out vec3 fs_Position;

void main()
{
	vec4 position = in_ModelViewMatrix * vec4(in_Position, 1.0);
	gl_Position = in_ProjectionMatrix * position;
	fs_Position = position.xyz;
	fs_Color = in_Color;
	fs_Normal = (in_ModelViewMatrix * vec4(in_Normal, 0.0)).xyz;
}