workspace "so-so"
	architecture "x64"
	startproject "Sandbox"

	configurations 
	{
		"Debug",
		"Release",
		"Dist"
	}


Outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- -- Include directories relative to root folder (solution directory)
-- IncludeDir = {}
-- IncludeDir["GLFW"] = "so-so/vendor/GLFW/include"
-- IncludeDir["Glad"] = "so-so/vendor/Glad/include"
-- IncludeDir["ImGui"] = "so-so/vendor/imgui"
-- IncludeDir["glm"] = "so-so/vendor/glm"


-- This includes the premake5.lua file from forked glfw repo into project "so-so"
include "so-so/vendor/GLFW"
include "so-so/vendor/Glad"
include "so-so/vendor/imgui"

project "so-so"
	location "so-so"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. Outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. Outputdir .. "/%{prj.name}")

	pchheader "sspch.h"
	pchsource "so-so/src/sspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",

		"%{prj.name}/vendor/stb_image/stb_image.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/stb_image"
	}

	links 
	{
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		
		systemversion "latest"

		defines 
		{
			"SS_PLATFORM_WINDOWS",
			"SS_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
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
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. Outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. Outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
    {
        "so-so/src",
        "so-so/vendor/spdlog/include",
        "so-so/vendor/GLFW/include",
        "so-so/vendor/Glad/include",
        "so-so/vendor/imgui",
        "so-so/vendor/glm",
        "so-so/vendor/stb_image"
    }

	links 
	{
		"so-so"
	}

	filter "system:windows"

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
