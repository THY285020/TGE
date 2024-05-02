#pragma once

#include<glm/glm.hpp>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

namespace TGE
{
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation,
			glm::vec3 outRotation, glm::vec3& outScale);
	}
}