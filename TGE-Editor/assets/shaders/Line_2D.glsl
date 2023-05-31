#type vertex
#version 450 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

uniform mat4 ViewProj;

struct VertexOutput
{
	vec4 Color;//16 28
};
layout(location = 0) out VertexOutput Output;

void main() {
	Output.Color = Color;

	gl_Position = ViewProj * vec4(Position, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec4 Color;
};

layout(location = 0) in VertexOutput Input;

uniform int entity_id;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int Entity_ID;

void main() {

	FragColor = Input.Color;

	Entity_ID = entity_id;
}