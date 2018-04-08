#version 330



uniform sampler2D in_Albedo;
uniform sampler2D in_Normal;
uniform sampler2D in_Edge;
uniform sampler2D in_Depth;
uniform vec3 in_LightDirection;
uniform vec3 in_LightColor;
uniform vec3 in_FogColor;

in vec2 fs_TexCoord;


float CalculateFogFactor(float t)
{
	return pow(t, 2.0);
}

void main()
{	
	vec4 sample = texture2D(in_Normal, fs_TexCoord);
	vec3 normal = normalize(sample.xyz * 2.0 - 1.0);
	float fogFactor = CalculateFogFactor(sample.w);
	float diffuseFactor = max(0.0, dot(normal, in_LightDirection));	

	//vec4 color = texture2D(in_Albedo, fs_TexCoord) * texture2D(in_Edge, fs_TexCoord) * vec4(in_LightColor.xyz, 1.0) * diffuseFactor;
	vec4 color = texture2D(in_Albedo, fs_TexCoord) * vec4(in_LightColor.xyz, 1.0) * diffuseFactor;

	gl_FragColor = mix(color, vec4(in_FogColor, 1.0), fogFactor);
}