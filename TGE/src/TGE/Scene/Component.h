#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "TGE/Renderer/SceneCamera.h"
#include "TGE/Core/TimeStep.h"
#include "ScriptableEntity.h"
#include "TGE/Renderer/Texture.h"

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
		/*glm::mat4 Transform = glm::mat4(1.f);*/
		glm::vec3 Translate = { 0.f, 0.f, 0.f };
		glm::vec3 Rotation =  { 0.f, 0.f, 0.f };
		glm::vec3 Scale =     { 1.f, 1.f, 1.f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)
			:Translate(translation) {}
		glm::mat4 GetTransform()const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			/*glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1,0,0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0,1,0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0,0,1 });*/

			return glm::translate(glm::mat4(1.0f), Translate)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.f, 1.f, 1.f, 1.f};

		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

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

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InitScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		//std::function<void(ScriptableEntity*)> OnCreateFunc;
		//std::function<void(ScriptableEntity*, TimeStep)> OnUpdateFunc;
		//std::function<void(ScriptableEntity* )> OnDestroyFunc;

		template<typename T>
		void Bind()
		{

			InitScript = []() {return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) {delete nsc->Instance; nsc->Instance = nullptr; };

			//OnCreateFunc = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
			//OnUpdateFunc = [](ScriptableEntity* instance, TimeStep ts) { ((T*)instance)->OnUpdate(ts); };
			//OnDestroyFunc = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
		}
	};
}