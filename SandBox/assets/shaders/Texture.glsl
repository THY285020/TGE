#type vertex
#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

uniform mat4 ViewProj;
uniform mat4 Transfrom;
out vec2 f_TexCoord;

void main() {
	f_TexCoord = TexCoord;
	gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
}

#type fragment
#version 330 core
in vec2 f_TexCoord;

uniform sampler2D Texture;

out vec4 FragColor;

void main() {
	//FragColor = vec4(f_TexCoord, 0.0f, 1.0f);
	FragColor = texture(Texture, f_TexCoord);
}