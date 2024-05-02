#type vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aColor;

struct VertexOutput
{
	vec2 TexCoord;
	vec3 Color;
};

uniform mat4 ViewProj;
uniform mat4 Transform;

layout(location = 0) flat out VertexOutput Output;

void main() {
	Output.TexCoord = aTexCoord;
	Output.Color = aColor;

	gl_Position = ViewProj * Transform * vec4(aPos, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec2 TexCoord;
	vec3 Color;
};

layout(location = 0) in VertexOutput Input;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int entity_id;

void main() {

	vec2 uv = Input.TexCoord * 100;//tiling

	//��ȡС��
	uv.x = uv.x - int(uv.x);
	uv.y = uv.y - int(uv.y);

	uv = abs(uv - 0.5);
	float min = min(uv.x, uv.y);
	float isShader = 1.0 - step(0.005, min);//С��0.05����Ե�������

	//����1
	vec3 color = Input.Color * isShader;

	FragColor = vec4(Input.Color, isShader);

	//����2
	//vec4 color = vec4(Input.Color, 0.0);
	//if (min <= 0.005)
	//{
	//	color.a = 1.0;
	//}
	//FragColor = color;

	entity_id = -1;
}