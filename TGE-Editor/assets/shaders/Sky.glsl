#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 viewProj;

layout(location = 0) out vec3 wPos;
layout(location = 1) out vec2 uv;


void main()
{
	gl_Position = viewProj * model * vec4(aPosition, 1.0);
	wPos = aPosition;
	uv = aTexCoord;
}

#type fragment
#version 450 core

layout(location = 0) in vec3 wPos;
layout(location = 1) in vec2 uv;

layout(binding = 0)uniform sampler2D skyTex;

uniform float turbidity;
uniform float exposure;
uniform float thetaS;
uniform vec4 SunPos;
uniform vec3 zenith;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int entity_id;

float Luminance(vec3 color)
{
	return dot(color, vec3(0.0396819152f, 0.458021790f, 0.00609653955f));
}

void main()
{
	float SunSize = 0.9997;
	vec3 vPos = normalize(wPos);
	vec4 dayColor = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 nightColor = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	nightColor = texture(skyTex, uv);
	nightColor.rgb = pow(nightColor.rgb, vec3(2.2f));

	if (SunPos.y <= 0.0f)
	{
		FragColor = nightColor;
		return;
	}

	if (vPos.y < -0.01f)//об╡Ц
	{
		FragColor = vec4(0.5f, 0.5f, 0.5f, 0.0f);
		return;
	}

	vPos.y = abs(vPos.y);
	float cosTheta = dot(vPos, up);

	vec3 vSunPos = normalize(SunPos.xyz);
	float sDp = dot(vPos, vSunPos);
	float gamma = acos(sDp);

	vec3 A = vec3(-0.0193f * turbidity - 0.2592f, -0.0167f * turbidity - 0.2608f, 0.1787f *turbidity- 1.4630f);
	vec3 B = vec3(-0.0665f * turbidity + 0.0008f, -0.0950f * turbidity + 0.0092f, -0.3554f *turbidity + 0.4275f);
	vec3 C = vec3(-0.0004f * turbidity + 0.2125f, -0.0079f * turbidity + 0.2102f, -0.0227f *turbidity + 5.3251f);
	vec3 D = vec3(-0.0641f * turbidity - 0.8989f, -0.0441f * turbidity - 1.6537f, 0.1206f *turbidity - 2.5771f);
	vec3 E = vec3(-0.0033f * turbidity + 0.0452f, -0.0109f * turbidity + 0.0529f, -0.0670f *turbidity + 0.3703f);

	vec3 F1 = (1.0f + A * exp(B / cosTheta)) * (1.0f + C * exp(D * gamma) + E * pow(sDp, 2.0f));
	vec3 F2 = (1.0f + A * exp(B)) * (1.0f + C * exp(D * thetaS) + E * pow(cos(thetaS), 2.0f));
	
	vec3 xyY = zenith * (F1 / F2);
	xyY.z = 1.0f - exp((1.0f / -exposure) * xyY.z);

	//xyY to XYZ
	vec3 XYZ = vec3(xyY.x * xyY.z / xyY.y, xyY.z, (1.0f - xyY.x - xyY.y) * xyY.z / xyY.y);

	//XYZ to RGB
	dayColor = vec4(3.240479f * XYZ.x - 1.53715f * XYZ.y - 0.49853f * XYZ.z,
		-0.969256f * XYZ.x + 1.875991f * XYZ.y + 0.041556f * XYZ.z,
		0.055648f * XYZ.x - 0.204043f * XYZ.y + 1.057311f * XYZ.z,
		1.0f);

	if (sDp > SunSize)
	{
		dayColor.rgb = dayColor.rgb * 5.0f;
	}

	float lum = Luminance(dayColor.rgb * 10.0f);
	lum = clamp(lum, 0.0f, 1.0f);
	FragColor = dayColor + nightColor * (1.0f - lum);
	entity_id = -1;
}