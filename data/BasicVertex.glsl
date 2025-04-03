#version 330 core

uniform mat4 matViewProjection;
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec2 uv;

struct VertexData
{
	vec4 colour;
};

out VertexData vertices;

void main(void)
{
	gl_Position = matViewProjection * vec4(position, 1.0);
	gl_Position.x += sin(position.x) * 0.5;
	gl_Position.y += cos(position.y) * 0.5;
	vertices.colour = vec4(abs(sin(gl_Position.x*gl_Position.y*0.15)),abs(cos(gl_Position.y*0.2)),abs(tan(gl_Position.x*0.25)), 1.0);
}
