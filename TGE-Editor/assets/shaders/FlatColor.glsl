//Flat Color Shader
#type vertex
#version 330 core
layout(location = 0) in vec3 Position;

uniform mat4 ViewProj;
uniform mat4 Transfrom;

void main() {
	gl_Position = ViewProj * Transfrom * vec4(Position, 1.0);
}

#type fragment
#version 330 core

uniform vec4 Color;
out vec4 FragColor;

void main() {
	//FragColor = vec4(f_TexCoord, 0.0f, 1.0f);
	FragColor = uColor;
}