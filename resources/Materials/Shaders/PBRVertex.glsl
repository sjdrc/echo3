#version 330 core
uniform mat4 matViewProjection;
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colour;
layout (location = 3) in vec2 texCoords;
layout (location = 4) in vec3 tangent;

out vec3 WorldPos;
out vec3 Normal;
out vec4 Colour;
out vec2 TexCoords;

// Parallex Mapping
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out mat3 TangentBitangentNormal;

uniform vec3 camPos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(void)
{
	WorldPos = vec3(model * vec4(position.xyz, 1.0));
	Normal = normal.xyz;
	TexCoords = texCoords;
	Colour = colour;

    vec3 T   = normalize(mat3(model) * tangent);
    vec3 N   = normalize(mat3(model) * normal);
    vec3 B   = cross(T, N);
    TangentBitangentNormal = transpose(mat3(T, B, N));	
	TangentViewPos = TangentBitangentNormal * camPos;
	TangentFragPos = TangentBitangentNormal * WorldPos;
 
	gl_Position = projection * view * vec4(WorldPos, 1.0);
}
