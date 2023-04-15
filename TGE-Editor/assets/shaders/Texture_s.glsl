#type vertex
#version 450 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color;
layout(location = 3) in float TexIndex;
layout(location = 4) in float TilingFactor;

uniform mat4 ViewProj;
//uniform mat4 Transfrom;
out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;//flat不进行插值
out float v_TilingFactor;

void main() {
	v_TexCoord = TexCoord;
	v_Color = Color;
	v_TexIndex = TexIndex;
	v_TilingFactor = TilingFactor;
	//gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
	gl_Position = ViewProj * vec4(Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int Entity_ID;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;
in float v_TilingFactor;
//uniform vec4 Color;
uniform sampler2D Textures[32];
uniform int entity_id;

void main() {
	//FragColor = vec4(f_TexCoord, 0.0f, 1.0f);
	FragColor = texture(Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
	Entity_ID = entity_id;
	//FragColor = v_Color;
}