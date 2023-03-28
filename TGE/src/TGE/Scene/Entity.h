#pragma once
#include "entt.hpp"
#include"Scene.h"

namespace TGE
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity m_id, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_id);
		}
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			TGE_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");
			return m_Scene->m_Registry.emplace<T>(m_id, std::forward<Args>(args)...);
		}
		template<typename T>
		T& GetComponent()
		{
			TGE_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
			return m_Scene->m_Registry.get<T>(m_id);
		}
		template<typename T>
		T& RemoveComponent()
		{
			TGE_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
			return m_Scene->m_Registry.remove<T>(m_id);
		}

		operator bool()const { return m_id != entt::null; }
	private:
		entt::entity m_id{ entt::null };
		Scene* m_Scene = nullptr;
	};
}


