workspace "TGE"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"--Debug-windows-x86_64

IncludeDir = {}
IncludeDir["GLFW"] = "TGE/vendor/GLFW/include"
IncludeDir["Glad"] = "TGE/vendor/Glad/include"
IncludeDir["ImGui"] = "TGE/vendor/imgui"
IncludeDir["glm"] = "TGE/vendor/glm"
IncludeDir["stb_image"] = "TGE/vendor/stb_image"

--复制了里面的premake5
include "TGE/vendor/GLFW"
include "TGE/vendor/Glad"
include "TGE/vendor/imgui"

project "TGE"
	location "TGE"
	kind "StaticLib"--动态则SharedLib
	language"C++"
	cppdialect"C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	pchheader "tgpch.h"
	pchsource "TGE/src/tgpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}
	
	links{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines--预处理
		{
			"TGE_PLATFORM_WINDOWS",
			"TGE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}


	filter "configurations:Debug"
		defines "TGE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TGE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "TGE_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language"C++"
	cppdialect"C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs{
		"TGE/vendor/spdlog/include",
		"TGE/src",
		"TGE/vendor",
		"%{IncludeDir.glm}"
	}
	
	links
	{
		"TGE"
	}

	filter "system:windows"
		systemversion "latest"

	defines
	{
		"TGE_PLATFORM_WINDOWS"	
	}
		
	filter "configurations:Debug"
		defines "TGE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "TGE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "TGE_DIST"
		runtime "Release"
		optimize "on"