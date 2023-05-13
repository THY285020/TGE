#include "tgpch.h"
#include "UUID.h"
#include <random>
#include <unordered_map>
namespace TGE
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());//种子由random_device提供
	static std::uniform_int_distribution<uint64_t> s_UniformDistrubution;

	//static std::unordered_map<uint64_t, std::string> m_Map;
	//static void AddToMap()
	//{
	//	m_Map[(uint64_t)TGE::UUID()] = "TGE";
	//}
	UUID::UUID()
	{
		m_UUID = s_UniformDistrubution(s_Engine);
	}

	UUID::UUID(uint64_t n):m_UUID(n)
	{
	}
}

