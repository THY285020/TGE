#include "tgpch.h"
#include "SubTexture2D.h"

namespace TGE
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D> Texture, const glm::vec2& min, const glm::vec2& max)
		:m_Texture(Texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}
	//材质，元素的左下位置，单元格大小，元素所占单元格
	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D> texture, const glm::vec2& coords, const glm::vec2& CellSize, const glm::vec2& ElementSize)
	{
		glm::vec2 min = { (coords.x * CellSize.x) / texture->GetWidth(), (coords.y * CellSize.y) / texture->GetHeight() };
		glm::vec2 max = { ((coords.x + ElementSize.x) * CellSize.x) / texture->GetWidth(), ((coords.y + ElementSize.y) * CellSize.y) / texture->GetHeight() };
		return std::make_shared<SubTexture2D>(texture, min, max);
	}
}

