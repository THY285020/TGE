#pragma once
#include "OrthoCamera.h"
#include "TGE/Core/TimeStep.h"

#include "TGE/Core/Application.h"
#include "TGE/Events/MouseEvent.h"

namespace TGE
{
	class OrthoCameraController
	{
	public:
		OrthoCameraController(float AspectRatio, bool Rotation = false);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& e);
		OrthoCamera& GetCamera() { return m_Camera; };

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() const { return m_ZoomLevel; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		//先创建上面长宽比和Zoom等级再用两个参数创建相机
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthoCamera m_Camera;

		bool m_Rotation = false;

		glm::vec3 m_CameraPos;
		float m_CameraRot = 0.0f;
		float m_CameraSpeed = 10.f;
		float m_CameraRotSpeed = 180.0f;
	};
}