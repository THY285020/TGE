#pragma once
#include "entt.hpp"
#include "TGE/Core/TimeStep.h"
#include "TGE/Core/UUID.h"
#include "TGE/Scene/Component.h"
#include "TGE/Renderer/EditorCamera.h"

class b2World;
namespace TGE
{
	class EditorCamera;
	class Serializer;
	class Entity;
	class Terrain;
	class Scene
	{
	public:
		Scene();
		Scene(uint32_t Width, uint32_t Height)
			:m_ViewportWidth(Width), m_ViewportHeight(Height)
		{}
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name, glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f));
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = "Empty Entity", glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f));
		void DestroyEntity(Entity entity);
		Entity DuplicateEntity(Entity entity);

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateSimulation(TimeStep ts, EditorCamera& camera);
		void OnUpdateRunTime(TimeStep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnSimulationStart();
		void OnSimulationStop();

		Entity GetPrimaryCamera();

		void UpdateEditCamera(glm::vec3 transform) {
			m_UpdateTransform = transform; 
			CameraNeedUpdate = true;
		}

		template<typename ...Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		bool CameraNeedUpdate = false;
		glm::vec3 m_UpdateTransform;

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
		void RenderScene(EditorCamera& camera);
		void OnPhysics2DStart();
		void OnPhysics2DStop();
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class Serializer;
	};
}


