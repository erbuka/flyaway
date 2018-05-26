#version 330

uniform vec3 in_ViewDirection;
uniform vec3 in_TopColor;
uniform vec3 in_HorizonColor;
uniform vec2 in_Resolution;

in vec2 fs_TexCoord;

void main()
{
	
	vec2 p = (fs_TexCoord * 2.0 - 1.0) * in_Resolution.xy / in_Resolution.yy;
	vec3 ray = normalize(vec3(p.x, p.y, -1.0));
	
	vec3 viewForward = -normalize(in_ViewDirection);
	vec3 viewRight = cross(vec3(0.0, 1.0, 0.0), viewForward);
	vec3 viewUp = cross(viewForward, viewRight);

	mat3 matRot = mat3(viewRight, viewUp, viewForward);

	vec3 rotRay = matRot * ray;
	
	float colorFactor = (dot(rotRay, vec3(0.0, 1.0, 0.0)) + 1.0) / 2.0;

	gl_FragData[0] = vec4(mix(in_HorizonColor, in_TopColor, colorFactor), 1.0);
}