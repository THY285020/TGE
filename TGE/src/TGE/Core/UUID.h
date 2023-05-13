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

//����UUID���͵Ĺ�ϣ����
namespace std
{
	template <typename T> struct hash;

	template<>
	struct hash<TGE::UUID>
	{
		size_t operator()(const TGE::UUID& uuid)const
		{
			return (uint64_t)uuid;//std::hash<T>()�������ع�ϣֵ������Ϊsize_t
		}
	};
}


