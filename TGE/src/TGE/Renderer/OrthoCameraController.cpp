#include "tgpch.h"
#include "OrthoCameraController.h"
#include "TGE/Core/Input.h"
#include "TGE/Core/KeyCodes.h"

namespace TGE {
	OrthoCameraController::OrthoCameraController(float AspectRatio, bool Rotation):
		m_AspectRatio(AspectRatio),
		m_Camera(-m_AspectRatio*m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_Rotation(Rotation),
		m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel })
	{
	}

	void OrthoCameraController::OnUpdate(TimeStep ts)
	{
		if (Input::IsKeyPressed(TGE_KEY_W) * ts)
		{
			m_CameraPos.z -= m_CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(TGE_KEY_S) * ts)
		{
			m_CameraPos.z += m_CameraSpeed * ts;
		}
		//为了使移动方向不变，需要根据镜头旋转角度调整（向量推导）
		if (Input::IsKeyPressed(TGE_KEY_A) * ts)
		{
			m_CameraPos.x -= cos(glm::radians(m_CameraRot))* m_CameraSpeed * ts;
			m_CameraPos.y -= sin(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(TGE_KEY_D) * ts)
		{
			m_CameraPos.x += cos(glm::radians(m_CameraRot))* m_CameraSpeed * ts;
			m_CameraPos.y += sin(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;
		}

		if (Input::IsKeyPressed(TGE_KEY_Q) * ts)
		{
			m_CameraPos.x -= sin(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;
			m_CameraPos.y += cos(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(TGE_KEY_E))
		{
			m_CameraPos.x += sin(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;
			m_CameraPos.y -= cos(glm::radians(m_CameraRot)) * m_CameraSpeed * ts;


		}
		//------------Rotation--------------------
		if (m_Rotation)
		{
			if (Input::IsKeyPressed(TGE_KEY_R))
			{
				m_CameraRot += m_CameraRotSpeed * ts;

				if (m_CameraRot > 180.f)
					m_CameraRot -= 360.f;
				else if (m_CameraRot <= -180.f)
					m_CameraRot += 360.f;
			}
			m_Camera.SetRotation(m_CameraRot);
		}

		m_Camera.SetPosition(m_CameraPos);
		m_CameraSpeed = m_ZoomLevel;
	}
	void OrthoCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(TGE_BIND_EVENT_FN(OrthoCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(TGE_BIND_EVENT_FN(OrthoCameraController::OnWindowResized));
	}
	bool OrthoCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.5f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		return false;
	}
	bool OrthoCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		return false;
	}
}