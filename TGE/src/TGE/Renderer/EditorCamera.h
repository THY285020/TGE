#pragma once
#include "TGE.h"
#include "Camera.h"
#include "TGE/Core/TimeStep.h"
#include "TGE/Events/Event.h"
#include "TGE/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace TGE
{
	class EditorCamera:public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetPosition()const { return m_Position; }
		glm::quat GetOrientation() const;

		glm::mat4 GetViewMatrix() const { return m_ViewMatrix;}
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		void UpdateProjection();

		void UpdateView();

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		bool OnMouseScroll(MouseScrolledEvent& e);
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;
		

	private:
		glm::vec2 m_InitialMousePosition;
		glm::vec3 m_Position = {0.f, 0.f, 0.f};
		glm::mat4 m_ViewMatrix;

		float m_NearClip = 0.1f;
		float m_FarClip = 1000.f;
		float m_AspectRatio = 1.778f;
		float m_Fov = 45.f;

		float m_Distance = 10.f;
		float m_ViewportWidth;
		float m_ViewportHeight;
		float m_Yaw = 0.f;
		float m_Pitch = 0.f;
		glm::vec3 m_FocalPoint = { 0.f, 0.f, 0.f };

	};
}

