#pragma once
#include "Light.h"
#include "glm/gtc/type_ptr.hpp"

//ÀÁººÄ£Ê½
namespace TGE
{
	class LightManager
	{
	private:
		LightManager() :lightID(0) {};

		static LightManager* ins;
		std::unordered_map<unsigned int, Ref<Light>> Lights;
		unsigned int lightID;

	public:
		static LightManager* Instance();

		unsigned int CreateLight(LightType type);
		void DeleteLight(unsigned int index);
		Ref<Light> GetLight(unsigned int index);

		~LightManager();
	};
}


