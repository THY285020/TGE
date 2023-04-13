#pragma once
#include "entt.hpp"
#include "TGE/Core/TimeStep.h"
#include "TGE/Scene/Component.h"
#include "TGE/Renderer/EditorCamera.h"

namespace TGE
{
	class EditorCamera;
	class Serializer;
	class Entity;
	class Scene
	{
	public:
		Scene();
		Scene(uint32_t Width, uint32_t Height)
			:m_ViewportWidth(Width), m_ViewportHeight(Height)
		{}
		~Scene();
		Entity CreateEntity(const std::string& name, glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f));
		void DestroyEntity(Entity entity);
		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRunTime(TimeStep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCamera();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		friend class Entity;
		friend class SceneHierarchyPanel;
		friend class Serializer;
	};
}


