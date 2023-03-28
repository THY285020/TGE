#include "tgpch.h"
#include "Entity.h"

namespace TGE
{
	Entity::Entity(entt::entity handle, Scene* scene)
		:m_id(handle),m_Scene(scene)
	{

	}
}

