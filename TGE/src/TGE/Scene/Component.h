#pragma once
#include <glm/glm.hpp>

#include "TGE/Renderer/SceneCamera.h"
namespace TGE
{
	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}
	};
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.f);

		TransformComponent() = default;
		TransformComponent(const glm::mat4 & transform)
			:Transform(transform) {}
		operator const glm::mat4& () { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.f, 1.f, 1.f, 1.f};

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			:Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

	};
}