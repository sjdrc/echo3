#version 330 core
precision mediump float;

struct VertexData
{
	vec4 colour;
};

in VertexData fragment;
out vec4 fragColor;

void main()
{
	fragColor = fragment.colour;
}
