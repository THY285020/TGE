#include "tgpch.h"
#include "Scene.h"
#include "TGE/Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include "Entity.h"
//Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
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

			Renderer2D::DrawSprite(transform.GetTransform(), sprite, int(entity));
			//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			//Renderer2D::SetEntity(int(entity));//要放在后面因为DrawQuad会调用FlushAndReset()重置
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

		//Physics
		{
			const int32_t velocityIteration = 2;
			const int32_t positionIteration = 6;
			m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);

			//改变物体位置
			auto view = m_Registry.view<RigidBody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& pos = body->GetPosition();
				transform.Translate.x = pos.x;
				transform.Translate.y = pos.y;
				transform.Rotation.z = body->GetAngle();

			}
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

				//Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, int(entity));
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
	void Scene::OnRuntimeStart()
	{
		m_PhysicsWorld = new b2World({0.f, -9.8f});//重力加速度
		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& tr = entity.GetComponent<TransformComponent>();
			auto& rg2d = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = b2BodyType(rg2d.Type);
			bodyDef.position.Set(tr.Translate.x, tr.Translate.y);
			bodyDef.angle = tr.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rg2d.FixedRotation);
			rg2d.RuntimeBody = body;//存储

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape pgShape;
				pgShape.SetAsBox(bc2d.Size.x * tr.Scale.x, bc2d.Size.y * tr.Scale.y);

				b2FixtureDef fd;
				fd.shape = &pgShape;
				fd.friction =	bc2d.Friction;
				fd.density =	bc2d.Density;
				fd.restitution = bc2d.Restitution;
				fd.restitutionThreshold = bc2d.RestitutionThreshold;

				body->CreateFixture(&fd);
				bc2d.FixtureRuntime = &fd;//存储
			}
		}
	}
	void Scene::OnRuntimeStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
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
		//Entity entity = { m_Registry.create(), this };
		//if (name.empty())
		//{
		//	entity.AddComponent<TagComponent>("Entity");
		//}
		//else
		//{
		//	entity.AddComponent<TagComponent>(name);
		//}
		//entity.AddComponent<IDComponent>();
		//entity.AddComponent<TransformComponent>(translation);
		//return entity;
		return CreateEntityWithUUID(UUID(), name, translation);
	}
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name, glm::vec3 translation)
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
		entity.AddComponent<IDComponent>(uuid);
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
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
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
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
	{
	}
}