#version 330


layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec3 in_Color;
layout(location = 3) in vec3 in_Extra0;

out vec3 gs_Color;
out vec3 gs_Extra0;

void main()
{
	gl_Position = vec4(in_Position, 1.0);
	gs_Color = in_Color;
	gs_Extra0 = in_Extra0;
}