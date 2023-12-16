#include "tgpch.h"
#include "LightManager.h"

namespace TGE
{
	LightManager* ins = nullptr;

	LightManager* LightManager::Instance()
	{
		if (ins == nullptr)
			ins = new LightManager();
		return ins;
	}

	unsigned int LightManager::CreateLight(LightType type)
	{
		unsigned int newID = lightID;
		switch (type)
		{
		case LightType::Direction:
			Lights[lightID] = std::make_shared<DirectionLight>();
			break;
		case LightType::Point:
			Lights[lightID] = std::make_shared<PointLight>();
			break;
		case LightType::Spot:
			Lights[lightID] = std::make_shared<SpotLight>();
			break;
		default:
			TGE_ASSERT(0, "Unsupported Light Type!");
			break;
		}

		lightID++;
		return newID;
	}

	void LightManager::DeleteLight(unsigned int id)
	{
		Lights.erase(id);
	}

	Ref<Light> LightManager::GetLight(unsigned int id)
	{
		return Lights[id];
	}

	LightManager::~LightManager()
	{
		Lights.clear();
		ins = nullptr;
	}
}