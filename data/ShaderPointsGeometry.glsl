#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

struct VertexData
{
	vec4 colour;
};

in VertexData vertices[];
out VertexData fragment;

uniform float pointSize;

void main(void)
{
	fragment.colour = vertices[0].colour;

	float voxHalfSize = pointSize * 0.5;

	gl_Position = gl_in[0].gl_Position+ vec4(-voxHalfSize,voxHalfSize,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(voxHalfSize,voxHalfSize,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-voxHalfSize,-voxHalfSize,0,0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(voxHalfSize,-voxHalfSize,0,0);
	EmitVertex();
	
	EndPrimitive();
}
