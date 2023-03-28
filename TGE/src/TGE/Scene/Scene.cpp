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
	void Scene::OnUpdate(TimeStep ts)
	{
		//Render Scene
		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;

		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			//��ȡComponentʵ��
			auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.camera;
				mainTransform = &transform.Transform;
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *mainTransform);
			//����ͬʱ�������������group
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
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
	Entity Scene::CreateEntity(const std::string& name)
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
		return entity;
	}
}