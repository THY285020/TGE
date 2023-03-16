#include "tgpch.h"
#include "OrthoCameraController.h"
#include "TGE/Core/Input.h"
#include "TGE/Core/KeyCodes.h"

namespace TGE {
	OrthoCameraController::OrthoCameraController(float AspectRatio, bool Rotation):
		m_AspectRatio(AspectRatio),
		m_Camera(-m_AspectRatio*m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_Rotation(Rotation)
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

		if (Input::IsKeyPressed(TGE_KEY_A) * ts)
		{
			m_CameraPos.x -= m_CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(TGE_KEY_D) * ts)
		{
			m_CameraPos.x += m_CameraSpeed * ts;
		}

		if (Input::IsKeyPressed(TGE_KEY_Q) * ts)
		{
			m_CameraPos.y += m_CameraSpeed * ts;
		}
		else if (Input::IsKeyPressed(TGE_KEY_E))
		{
			m_CameraPos.y -= m_CameraSpeed * ts;
		}
		//------------Rotation--------------------
		if (m_Rotation)
		{
			if (Input::IsKeyPressed(TGE_KEY_R))
			{
				m_CameraRot += m_CameraRotSpeed * ts;
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
		m_ZoomLevel -= e.GetYOffset() * 0.5;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthoCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}