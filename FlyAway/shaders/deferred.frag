#version 330

in vec3 fs_Color;
in vec3 fs_Normal;


void main()
{		
	gl_FragData[0] = vec4(fs_Color, 1.0);
	gl_FragData[1] = vec4((normalize(fs_Normal) + 1.0) / 2.0, 1.0);
}