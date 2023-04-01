#pragma once
#include"Entity.h"
namespace TGE
{
	class Entity;
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity(){}
		template<typename T>
		T& GetComponent()
		{
			return m_Entity->GetComponent<T>();
		}
	protected:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnUpdate(TimeStep ts) = 0;
	private:
		Entity* m_Entity;
		friend class Scene;
	};
}


