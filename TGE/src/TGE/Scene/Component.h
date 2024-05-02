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
#include "TGE/Core/UUID.h"

namespace TGE
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(uint64_t id) :ID(id)
		{
		}
	};
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
			//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1,0,0 })
			//	* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0,1,0 })
			//	* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0,0,1 });

			return glm::translate(glm::mat4(1.0f), Translate)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.f, 1.f, 1.f, 1.f};

		bool usingTexture = false;
		Ref<Texture2D> Texture = Texture2D::Create();
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			:Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.f, 1.f, 1.f, 1.f };
		float Thickness = 1.0f;//1.0是实心圆，0.5是圆环
		float Fade = 0.005f;//淡化

		//bool usingTexture = false;
		//Ref<Texture2D> Texture = Texture2D::Create();
		//float TilingFactor = 1.0f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color)
			:Color(color) {}
	};

	struct CubeRendererComponent
	{
		glm::vec4 Color{ 1.f, 1.f, 1.f, 1.f };

		bool usingTexture = false;
		Ref<Texture2D> Texture = Texture2D::Create();
		//float TilingFactor = 1.0f;

		CubeRendererComponent() = default;
		CubeRendererComponent(const CubeRendererComponent&) = default;
		CubeRendererComponent(const glm::vec4& color)
			:Color(color) {}
	};

	struct SphereRendererComponent
	{
		glm::vec4 Color{ 1.f, 1.f, 1.f, 1.f };

		bool usingTexture = false;
		Ref<Texture2D> Texture = Texture2D::Create();

		SphereRendererComponent() = default;
		SphereRendererComponent(const SphereRendererComponent&) = default;
		SphereRendererComponent(const glm::vec4& color)
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

	struct RigidBody2DComponent
	{
		enum class BodyType { Static = 0, Kinematic, Dynamic};
		BodyType Type = BodyType::Static;

		bool FixedRotation = false;
		//存储b2Body对象
		void* RuntimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.f, 0.f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.f;
		float Friction = 0.5f;
		//形变
		float Restitution = 0.0f;//衡量弹性的恢复系数，完全弹性碰撞为1
		float RestitutionThreshold = 0.5f;
		//存储
		void* FixtureRuntime = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.f, 0.f };
		float Radius = 0.5f;

		float Density = 1.f;
		float Friction = 0.5f;
		//形变
		float Restitution = 0.0f;//衡量弹性的恢复系数，完全弹性碰撞为1
		float RestitutionThreshold = 0.5f;
		//存储
		void* FixtureRuntime = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

}