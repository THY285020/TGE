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
		void SetFocusPos(glm::vec3 newPos) { 
			m_FocalPoint = newPos; 
			//m_Position = newPos + glm::vec3(0.0f, 0.0f, 2.5f); 因为position会根据focal_point改变 
			UpdateView(); 
		}

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetPosition()const { return m_Position; }
		glm::quat GetOrientation() const;

		glm::mat4 GetViewMatrix() const { return m_ViewMatrix;}
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		float GetMoveSpeed()const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = 0.01 * speed; }
	private:
		void UpdateProjection();

		void UpdateView();

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		bool OnMouseScroll(MouseScrolledEvent& e);
		inline bool OnMouseRelease(MouseButtonReleasedEvent& e);
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

		float m_MoveSpeed = 0.01f;
		bool firstMouse = true;

		float m_Distance = 10.f;
		float m_ViewportWidth;
		float m_ViewportHeight;
		float m_Yaw = 0.f;
		float m_Pitch = 0.f;
		glm::vec3 m_FocalPoint = { 0.f, 0.f, 0.f };

	};
}

