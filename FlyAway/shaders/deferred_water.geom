#version 330

uniform mat4 in_ProjectionMatrix;
uniform mat4 in_ModelViewMatrix;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gs_Color[3];
in vec3 gs_Extra0[3];

smooth out vec3 fs_Color;
smooth out vec3 fs_Normal;
smooth out vec3 fs_Position;

vec3 getVertex(vec3 original)
{
	return original;
}

void main()
{

	vec3 vertices[3];

	for(int i = 0; i < 3; i++)
	{
		vertices[i] = getVertex(gl_in[i].gl_Position.xyz);
	}

	vec3 v0 = vertices[1] - vertices[0];
	vec3 v1 = vertices[2] - vertices[1];

	vec3 normal = mat3(in_ModelViewMatrix) * normalize(cross(v0, v1));

	for(int i = 0; i < 3; i++)
	{
		float alpha = max(min(0.0, gs_Extra0[i].y), -1) / -1;
		vec4 position = in_ModelViewMatrix * vec4(vertices[i], 1.0);
		gl_Position = in_ProjectionMatrix * position;
		fs_Color = gs_Color[i];
		fs_Normal = normal;
		fs_Position = position.xyz;
		EmitVertex();
	}
	EndPrimitive();
}