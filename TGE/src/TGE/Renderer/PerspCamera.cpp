#include "tgpch.h"
#include "PerspCamera.h"

namespace TGE {
	PerspCamera::PerspCamera(
		float posX, float posY, float posZ, 
		float upX, float upY, float upZ, 
		float frontX, float frontY, float frontZ, 
		float near_plane, 
		float far_plane):NearPlane(near_plane), FarPlane(far_plane)
	{
		Position = glm::vec3(posX, posY, posZ);
		Up = glm::vec3(upX, upY, upZ);
		Front = glm::vec3(frontX, frontY, frontZ);

		ViewMatrix = glm::lookAt(Position, Position + Front, Up);
		ProjectionMatrix = glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}

	void PerspCamera::RecalViewMatrix()
	{
		/*glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0, 0.0, 1.0));*/

		//ViewMatrix = glm::inverse(transform);
		ViewMatrix = glm::lookAt(Position, Position + Front, Up);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}
	void PerspCamera::RecalProjectionMatrix()
	{
		ProjectionMatrix = glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}
}