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
		entt::entity entity= m_Registry.create();//entity������uint32
		//�ѵ�λ���󸽼ӵ�entity��,�᷵�ؾ������� auto& transform = 
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		//Ҳ������get��ȡ
		TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		//�������к���TransformComponent��entity�����Ի�ȡ�����ԣ�transform��������
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		//����ͬʱ�������������group
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
		//ÿ��component�����������OnTransformConstruct����
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
#endif
		
	}
	Scene::~Scene()
	{
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);
		//����ͬʱ�������������group
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
			//��ȡComponentʵ��
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
			//����ͬʱ�������������group
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