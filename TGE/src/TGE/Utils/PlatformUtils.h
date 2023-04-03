#pragma once
#include <string>
namespace TGE
{
	class FileDialogs
	{
	public:
		//return empty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}