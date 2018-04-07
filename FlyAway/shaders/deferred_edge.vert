#version 330

layout(location = 0) in vec2 in_TexCoord;

smooth out vec2 fs_TexCoord;

void main()
{
	gl_Position = vec4(in_TexCoord * 2.0 - 1.0, 0.0, 1.0);	
	fs_TexCoord = in_TexCoord;
}