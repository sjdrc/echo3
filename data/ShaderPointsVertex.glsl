#version 330 core
uniform mat4 matViewProjection;

struct VertexData
{
	vec4 colour;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 colour;

out VertexData vertices;

void main(void)
{
	gl_Position = matViewProjection * vec4(position,1);
	vertices.colour = colour;
}
