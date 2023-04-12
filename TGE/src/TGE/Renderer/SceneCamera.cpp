#include "tgpch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
namespace TGE
{
	SceneCamera::SceneCamera()
	{
		ReCalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		ReCalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		ReCalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		ReCalculateProjection();
	}

	void SceneCamera::ReCalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Orthographic)
		{
			float orthoLeft = -0.5f * m_AspectRatio * m_OrthographicSize;
			float orthoRight = 0.5f * m_AspectRatio * m_OrthographicSize;
			float orthoBottom = -0.5f * m_OrthographicSize;
			float orthoTop = 0.5f * m_OrthographicSize;

			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop,
				m_OrthographicNear, m_OrthographicFar);
		}
		else
		{
			/*float* m16 = glm::value_ptr(m_Projection);
			float ymax, xmax;
			ymax = m_PerspectiveNear * tanf(m_PerspectiveFOV * 3.141592f / 180.0f);
			xmax = ymax * m_AspectRatio;

			float temp, temp2, temp3, temp4;
			temp = 2.0f * m_PerspectiveNear;
			temp2 = xmax - (-xmax);
			temp3 = ymax - (-ymax);
			temp4 = m_PerspectiveFar - m_PerspectiveNear;
			m16[0] = temp / temp2;
			m16[1] = 0.0;
			m16[2] = 0.0;
			m16[3] = 0.0;
			m16[4] = 0.0;
			m16[5] = temp / temp3;
			m16[6] = 0.0;
			m16[7] = 0.0;
			m16[8] = (xmax + (-xmax)) / temp2;
			m16[9] = (ymax + (-ymax)) / temp3;
			m16[10] = (-m_PerspectiveFar - m_PerspectiveNear) / temp4;
			m16[11] = -1.0f;
			m16[12] = 0.0;
			m16[13] = 0.0;
			m16[14] = (-temp * m_PerspectiveFar) / temp4;
			m16[15] = 0.0;

			m_Projection = glm::mat4{
				{m16[0], m16[1], m16[2], m16[3]},
				{m16[4], m16[5], m16[6], m16[7]},
				{m16[8], m16[9], m16[10], m16[11]},
				{m16[12], m16[13], m16[14], m16[15]}
			};*/
			//m_PerspectiveNear, m_PerspectiveFar m_OrthographicNear, m_OrthographicFar
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}

	}
}