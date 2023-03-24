#pragma once

#include "TGE.h"

//基本属性
struct ParticleProps
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem();

	void OnUpdate(TGE::TimeStep ts);
	void OnRender(TGE::OrthoCamera& camera);

	void Emit(const ParticleProps& particleProps);
private:
	//在基本属性基础上随机波动
	struct Particle
	{
		glm::vec2 Position;
		glm::vec2 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;//因为后渲染粒子覆盖在前面，所以从末尾开始设置粒子

};

