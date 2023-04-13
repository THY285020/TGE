#include "tgpch.h"
#include "Scene.h"
#include "TGE/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include "Entity.h"
namespace TGE
{
	static void DoMaths(const glm::mat4& transform)
	{

	}
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
	{

	}
	Scene::Scene()
	{
#if ENTT_EXAMPLE
		entt::entity entity= m_Registry.create();//entity本质是uint32
		//把单位矩阵附加到entity上,会返回矩阵引用 auto& transform = 
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		//也可以用get获取
		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		//遍历所有含有TransformComponent的entity，可以获取其属性（transform）的引用
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		//遍历同时带有两个组件的group
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
		//每次component创建都会调用OnTransformConstruct函数
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
#endif
		
	}
	Scene::~Scene()
	{
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);
		//遍历同时带有两个组件的group
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
		}
		Renderer2D::EndScene();

	}

	void Scene::OnUpdateRunTime(TimeStep ts)
	{
		//Update Script
		{
			//auto view = m_Registry.view<NativeScriptComponent>();
			//for (auto entity : view)
			//{
			//	auto& nsc = view.get<NativeScriptComponent>(entity);
			//	if (!nsc.Instance)
			//	{
			//		nsc.Instance = nsc.InitScript();
			//		/*nsc.Instance->m_Entity = &Entity{ entity, this };*/
			//		nsc.Instance->OnCreate();
			//	}
			//	
			//	nsc.Instance->OnUpdate(ts);
			//}
			m_Registry.view<NativeScriptComponent>().each([=](entt::entity entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InitScript();
					/*nsc.Instance->m_Entity = &Entity{ entity, this };*/
					nsc.Instance->OnCreate();
				}
				nsc.Instance->m_Entity = &Entity{ entity, this };
				nsc.Instance->OnUpdate(ts);
			});
		}
		//Render Scene
		Camera* mainCamera = nullptr;
		glm::mat4 mainTransform;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			//获取Component实例
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.camera;
				mainTransform = transform.GetTransform();
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, mainTransform);
			//遍历同时带有两个组件的group
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}
			Renderer2D::EndScene();
		}
		
	}
	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}
	Entity Scene::GetPrimaryCamera()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}
	Entity Scene::CreateEntity(const std::string& name, glm::vec3 translation)
	{
		Entity entity = { m_Registry.create(), this };
		if (name.empty())
		{
			entity.AddComponent<TagComponent>("Entity");
		}
		else
		{
			entity.AddComponent<TagComponent>(name);
		}
		entity.AddComponent<TransformComponent>(translation);
		return entity;
	}
	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);

	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
		
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}