workspace "TGE"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "TGE/vendor/GLFW/include"
IncludeDir["Glad"] = "TGE/vendor/Glad/include"
IncludeDir["ImGui"] = "TGE/vendor/imgui"
IncludeDir["glm"] = "TGE/vendor/glm"


--复制了里面的premake5
include "TGE/vendor/GLFW"
include "TGE/vendor/Glad"
include "TGE/vendor/imgui"

project "TGE"
	location "TGE"
	kind "SharedLib"
	language"C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .."/%{prj.name}")
	objdir ("bin-int/" .. outputdir .."/%{prj.name}")

	pchheader "tgpch.h"
	pchsource "TGE/src/tgpch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
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
		"%{IncludeDir.glm}"
	}
	
	links{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect"C++17"
		systemversion "latest"

		defines
		{
			"TGE_PLATFORM_WINDOWS",
			"TGE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands{
			-- %{cfg.buildtarget.relpath}是生成文件，相较于当前premake5.lua文件的相对路径
			-- ..是一种语法，output相当于之前声明的一个string变量
			--("{COPY}%{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
			--("{COPY} ../bin/" .. outputdir .. "/TGE/TGE.dll" .. " ../bin/" .. outputdir .. "/Sandbox")
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "TGE_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "TGE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "TGE_DIST"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language"C++"
	staticruntime "Off"

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
		cppdialect"C++17"
		systemversion "latest"

	defines
	{
		"TGE_PLATFORM_WINDOWS"	
	}
		
	filter "configurations:Debug"
		defines "TGE_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "TGE_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "TGE_DIST"
		buildoptions "/MD"
		optimize "On"