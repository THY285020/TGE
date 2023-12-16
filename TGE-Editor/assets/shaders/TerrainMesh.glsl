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

layout(location = 0) out VertexOutput Output;

void main() {
	Output.TexCoord = aTexCoord;
	Output.Color = aColor;

	gl_Position = ViewProj * vec4(aPos, 1.0);
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

void main() {

	vec2 uv = Input.TexCoord * 30;//tiling

	//截取小数
	uv.x = uv.x - int(uv.x);
	uv.y = uv.y - int(uv.y);
	
	uv = abs(uv - 0.5);
	float min = min(uv.x, uv.y);
	float isShader = 1.0 - step(0.05, min);//小于0.05（边缘）则绘制

	vec3 color = Input.Color * isShader;

	FragColor = vec4(color, 1.0);
}