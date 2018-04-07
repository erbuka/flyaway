#version 330



uniform sampler2D in_Albedo;
uniform sampler2D in_Normal;
uniform sampler2D in_Edge;
uniform vec3 in_LightDirection;
uniform vec3 in_LightColor;

in vec2 fs_TexCoord;

void main()
{	
	
	vec3 normal = normalize(texture2D(in_Normal, fs_TexCoord).xyz * 2.0 - 1.0);
	float diffuseFactor = max(0.0, dot(normal, in_LightDirection));	
	gl_FragColor = texture2D(in_Albedo, fs_TexCoord) * texture2D(in_Edge, fs_TexCoord) * vec4(in_LightColor.xyz, 1.0) * diffuseFactor;
	//gl_FragColor = texture2D(in_Normal, fs_TexCoord);
}