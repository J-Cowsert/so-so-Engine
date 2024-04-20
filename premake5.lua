workspace "so-so"
	architecture "x64"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "so-so/vendor/GLFW/include"


-- This includes the premake5.lua file from forked glfw repo into project "so-so"
include "so-so/vendor/GLFW"

project "so-so"
	location "so-so"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "sspch.h"
	pchsource "so-so/src/sspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/GLFW/include",
		"{IncludeDir.GLFW}"
	}

	links 
	{
		"GLFW",
		"opengl32.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"SS_PLATFORM_WINDOWS",
			"SS_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}
	
	filter "configurations:Debug"
		defines "SS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SS_DIST"
		runtime "Release"
		optimize "On"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"so-so/vendor/spdlog/include",
		"so-so/src"
	}

	links 
	{
		"so-so"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"SS_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "SS_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SS_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SS_DIST"
		runtime "Release"
		optimize "On"
