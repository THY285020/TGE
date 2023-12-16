#pragma once
#include <glm/gtc/type_ptr.hpp>

namespace TGE
{
	enum class LightType
	{
		Direction = 0,
		Point = 1,
		Spot = 2,
	};

	class Light
	{
	public:
		Light() :m_Pos(glm::vec3(0.0f)),m_Color(glm::vec3(1.0f)),m_NearClip(0.01f),m_FarClip(1000.f) {};
		void SetPos(glm::vec3 pos) { m_Pos = pos; }
		void SetColor(glm::vec3 color) { m_Color = color; }
	private:
		glm::vec3 m_Pos;
		glm::vec3 m_Color;
		float m_NearClip;
		float m_FarClip;

	};


	class DirectionLight : public Light
	{
	public:
		DirectionLight() :m_Dir(glm::vec3(0.0f)) {}

		inline void SetDir(glm::vec3& dir) { m_Dir = dir; }

	private:
		glm::vec3 m_Dir;
	};


	class PointLight : public Light
	{
	public:
		PointLight(){}
	};


	class SpotLight : public Light
	{
	public:
		SpotLight() {}

	};

}


