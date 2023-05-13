#pragma once

#include <xhash>
namespace TGE
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t n);
		UUID(const UUID&) = default;
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

//定义UUID类型的哈希函数
namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<TGE::UUID>
	{
		size_t operator()(const TGE::UUID& uuid)const
		{
			return (uint64_t)uuid;//std::hash<T>()函数返回哈希值，类型为size_t
		}
	};
}


