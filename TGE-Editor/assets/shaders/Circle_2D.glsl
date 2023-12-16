#type vertex
#version 450 core
layout(location = 0) in vec3 WorldPosition;
layout(location = 1) in vec3 LocalPosition;
layout(location = 2) in vec4 Color;
layout(location = 3) in float Thickness;
layout(location = 4) in float Fade;

uniform mat4 ViewProj;

struct VertexOutput
{
	vec3 LocalPosition;//12 12
	vec4 Color;//16 28
	float Thickness;//4 32
	float Fade;//4 36 每16个字节占用一个slot，共占用3个slot
};
layout(location = 0) out VertexOutput Output;

void main() {
	Output.LocalPosition = LocalPosition;
	Output.Color = Color;
	Output.Thickness = Thickness;
	Output.Fade = Fade;

	gl_Position = ViewProj * vec4(WorldPosition, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout(location = 0) in VertexOutput Input;

uniform int entity_id;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int Entity_ID;

void main() {
	//Calculate distance and fill circle with white
	//length得到当前像素到圆心的距离，1-length反转使得圆心处为1（白色）
	float distance = 1.0 - length(Input.LocalPosition);
	//step函数，step(a, x)大于a则为1
	float circleAlpha = smoothstep(0.0, Input.Fade, distance);//淡化边界，从0.0到fade逐渐为1（外边界）
	circleAlpha *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);//圆环（内边界）：距离大于厚度加淡化的长度为0，小于则逐渐到1，


	if (circleAlpha == 0)
		discard;
	FragColor = Input.Color;
	FragColor.a *= circleAlpha;

	Entity_ID = entity_id;

}