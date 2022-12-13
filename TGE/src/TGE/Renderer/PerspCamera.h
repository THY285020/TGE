#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>

namespace TGE {
	class PerspCamera
	{
	public:

		PerspCamera(glm::vec3 position = glm::vec3(0, 0, 0),
			glm::vec3 up = glm::vec3(0, 1, 0),
			glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
			float near_plane = 0.1f,
			float far_plane = 100.0f)
			: 
			Position(position), 
			Up(up), 
			Front(front),
			NearPlane(near_plane),
			FarPlane(far_plane)
		{
			ViewMatrix = glm::lookAt(Position, Position + Front, Up);
			ProjectionMatrix = glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}

		PerspCamera(float posX, float posY, float posZ,
			float upX, float upY, float upZ,
			float frontX, float frontY, float frontZ,
			float near_plane = 0.1f,
			float far_plane = 100.0f);

		void SetPosition(const glm::vec3& position) { Position = position; RecalViewMatrix(); }
		const glm::vec3& GetPosition() const { return Position; }
		
		void SetRotation(const float rotation) { Rotation = rotation; RecalViewMatrix(); }
		const float GetRotation() const { return Rotation; }

		void SetZoom(const float zoom) { Zoom = zoom; RecalViewMatrix(); }
		const float GetZoom() const { return Zoom; }

		const glm::mat4& GetViewMatrix()const { return ViewMatrix; }
		const glm::mat4& GetProjectionMatrix()const { return ProjectionMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }


	private:
		void RecalViewMatrix();

	private:
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ViewProjectionMatrix;

		float Rotation = 0.0f;
		float Zoom = 45.0f;
		float NearPlane;
		float FarPlane; 
		float AspectRatio = 1280.f/720.f;
	
	};
}


