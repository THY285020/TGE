#pragma once
#include"TGE/Renderer/Camera.h"
#include <glm/glm.hpp>
namespace TGE
{
	class SceneCamera: public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFov, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; ReCalculateProjection(); }
		float GetPerspectiveNearClip()const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; ReCalculateProjection(); }
		float GetPerspectiveFarClip()const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; ReCalculateProjection(); }

		float GetOrthographicSize()const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; ReCalculateProjection();}
		float GetOrthographicNearClip()const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; ReCalculateProjection(); }
		float GetOrthographicFarClip()const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; ReCalculateProjection();}

		ProjectionType GetProjectionType()const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; ReCalculateProjection();}
	private:
		void ReCalculateProjection();
	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = -1.0f;
		float m_PerspectiveFar = 1000.0f;

		float m_AspectRatio = 0.0f;
	};

}

