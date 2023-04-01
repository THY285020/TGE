#pragma once
#include "entt.hpp"
#include "TGE/Scene/Scene.h"
#include "TGE/Core/log.h"
namespace TGE
{
	class Scene;
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity m_id, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_id);
		}
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			TGE_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_id, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T>
		T& GetComponent()
		{
			TGE_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
			return m_Scene->m_Registry.get<T>(m_id);
		}
		template<typename T>
		void RemoveComponent()
		{
			TGE_CORE_ASSERT(HasComponent<T>(), "Entity does not have this component!");
			m_Scene->m_Registry.remove<T>(m_id);
		}

		operator bool()const { return m_id != entt::null; }
		operator uint32_t()const { return uint32_t(m_id); }
		operator entt::entity() const { return m_id; }//”√”⁄Sene::Destroy(Entity)

		bool operator ==(const Entity& other) const 
		{
			return m_id == other.m_id && m_Scene == other.m_Scene;
		}
		bool operator !=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_id{ entt::null };
		Scene* m_Scene = nullptr;
	};
}


