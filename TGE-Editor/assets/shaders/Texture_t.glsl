#type vertex
#version 450 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color;
layout(location = 3) in float TexIndex;
layout(location = 4) in float TilingFactor;

//layout(std140, binding = 0) uniform Camera
//{
//	mat4 ViewProj;
//}
uniform mat4 ViewProj;

struct VertexOutput
{
	vec2 TexCoord;
	vec4 Color;
	float TexIndex;//flat不进行插值
	float TilingFactor;
};
layout(location = 0) out VertexOutput Output;


void main() {
	Output.TexCoord = TexCoord;
	Output.Color = Color;
	Output.TexIndex = TexIndex;
	Output.TilingFactor = TilingFactor;

	gl_Position = ViewProj * vec4(Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int Entity_ID;

struct VertexOutput
{
	vec2 TexCoord;
	vec4 Color;
	float TexIndex;
	float TilingFactor;
};
layout(location = 0) in VertexOutput Input;

//uniform vec4 Color;
//uniform sampler2D Textures[32];
layout(binding = 0) uniform sampler2D Textures[32];
uniform int entity_id;

void main() {

	FragColor = texture(Textures[int(Input.TexIndex)], Input.TexCoord * Input.TilingFactor) * Input.Color;
	Entity_ID = entity_id;

}