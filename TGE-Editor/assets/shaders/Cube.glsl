#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
layout(location = 4) in float TexIndex;
layout(location = 5) in int ID;

uniform mat4 ViewProj;

struct VertexOutput
{
	vec3 Pos;
	vec2 TexCoord;
	vec3 Normal;
	vec4 Color;
};

layout(location = 0) out VertexOutput Output;
layout(location = 4) flat out float v_TexIndex;
layout(location = 5) flat out int entity_id;


void main() {
	Output.Pos = aPos;
	Output.TexCoord = aTexCoord;
	Output.Normal = aNormal;
	Output.Color = aColor;

	v_TexIndex = TexIndex;
	entity_id = ID;

	gl_Position = ViewProj * vec4(aPos, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec3 Pos;
	vec2 TexCoord;
	vec3 Normal;
	vec4 Color;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) flat in float v_TexIndex;
layout(location = 5) flat in int entity_id;

layout(binding = 0) uniform sampler2D Textures[32];//对应glActiveTexture的槽位
uniform vec3 cameraPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int Entity_ID;

void main() {

	//I = normalize(cameraPos - Input.Pos);
	//R = reflect(I, normalize(Input.Normal));

	FragColor = texture(Textures[int(v_TexIndex)], Input.TexCoord) * Input.Color;
	Entity_ID = entity_id;
	//FragColor = Input.Color;

}