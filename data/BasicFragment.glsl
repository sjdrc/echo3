#version 330 core
precision mediump float;

struct VertexData
{
	vec4 colour;
};

in VertexData vertices;
out vec4 fragColor;

void main()
{
	float odd = floor(mod(gl_FragCoord.y*0.25, 2.0))+0.5;
	fragColor = vec4(vertices.colour.x, vertices.colour.y, vertices.colour.z, odd);
}
