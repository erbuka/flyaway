#version 330



uniform sampler2D in_Albedo;
uniform sampler2D in_Normal;
uniform sampler2D in_Edge;
uniform vec3 in_LightColor;

in vec2 fs_TexCoord;


float CalculateFogFactor(float t)
{
	return pow(t, 2.0);
}

void main()
{	
	vec4 sample = texture2D(in_Normal, fs_TexCoord);
	vec4 albedo = texture2D(in_Albedo, fs_TexCoord);
	float fogFactor = CalculateFogFactor(sample.w);

	vec3 color = albedo.xyz * texture2D(in_Edge, fs_TexCoord).xyz * in_LightColor.xyz;
	
	gl_FragColor = vec4(color, (1.0 - fogFactor) * albedo.a);
}