#include "tgpch.h"
#include "EditorCamera.h"

#include "TGE/Core/Input.h"
#include "TGE/Core/KeyCodes.h"
#include "TGE/Core/MouseButtonCodes.h"

#include <glm/gtx/quaternion.hpp>

namespace TGE
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		:m_Fov(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov),aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}
	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
	}
	void EditorCamera::UpdateView()
	{
		//m_Yaw = m_Pitch = 0.0f // Lock the Camera's rotation for 2D
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}
	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		//移动速度，经验设置
		float x = std::min(m_ViewportWidth / 1000.f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.f, 2.4f);
		float yFactor = 0.0366f * (y*y) - 0.1778f * y + 0.3021f;

		return{ xFactor, yFactor };
	}
	float EditorCamera::RotationSpeed()const
	{
		return 0.8f;
	}
	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 30.2f;//40.2f
		distance = std::max(distance, 0.0f);
		float speed = distance * distance * 0.5;
		speed = std::min(speed, 100.f);//max = 100.f
		return speed;
	}
	void EditorCamera::OnUpdate(TimeStep ts)
	{

		if (Input::IsMouseButtonPressed(TGE_MOUSE_BUTTON_MIDDLE))//移动
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta;
			if (firstMouse)
			{
				m_InitialMousePosition = mouse;//第一次要更新初始坐标
				firstMouse = false;
			}
			delta = (mouse - m_InitialMousePosition);
			m_InitialMousePosition = mouse;

			MousePan(delta * 0.003f);
		}
		//glm::vec2 lastPos;
		if (Input::IsMouseButtonPressed(TGE_MOUSE_BUTTON_RIGHT))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta;
			if (firstMouse)
			{
				m_InitialMousePosition = mouse;//第一次要更新初始坐标
				firstMouse = false;
			}
			delta = (mouse - m_InitialMousePosition);
			m_InitialMousePosition = mouse;

			MouseRotate(delta * 0.003f);

			//{
			//	m_Yaw += delta.x;
			//	m_Pitch -= delta.y;
			//	glm::vec3 front;
			//	front.x = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));//sin(glm::radians(yaw))
			//	front.y = sin(glm::radians(m_Pitch));
			//	front.z = -cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));//cos
			//	front = glm::normalize(front);
			// //位置 朝向 相机上方向
			//	m_ViewMatrix = glm::lookAt(m_FocalPoint, m_FocalPoint + front, up);
			//}
					
			//方向键移动
			if (Input::IsKeyPressed(TGE_KEY_W))
			{
				m_FocalPoint += 2 * m_MoveSpeed * GetForwardDirection();
			}
			if (Input::IsKeyPressed(TGE_KEY_S))
			{
				m_FocalPoint -= 2 * m_MoveSpeed * GetForwardDirection();;
			}			
			if (Input::IsKeyPressed(TGE_KEY_A))
			{
				m_FocalPoint -= 2 * m_MoveSpeed * GetRightDirection();
			}			
			if (Input::IsKeyPressed(TGE_KEY_D))
			{
				m_FocalPoint += 2 * m_MoveSpeed * GetRightDirection();
			}
			if (Input::IsKeyPressed(TGE_KEY_Q))
			{
				m_FocalPoint += m_MoveSpeed * GetUpDirection();
			}
			if (Input::IsKeyPressed(TGE_KEY_E))
			{
				m_FocalPoint -= m_MoveSpeed * GetUpDirection();
			}
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(TGE_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(TGE_BIND_EVENT_FN(EditorCamera::OnMouseRelease));

	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	bool EditorCamera::OnMouseRelease(MouseButtonReleasedEvent& e)
	{
		firstMouse = true;
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.f : 1.f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection()const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection()const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}

