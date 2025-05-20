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
	cppdialect "C++23"
	staticruntime "on"

	targetdir ("bin/" .. Outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. Outputdir .. "/%{prj.name}")

	pchheader "sspch.h"
	pchsource "so-so/src/sspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

		"%{prj.name}/vendor/glm/glm/**.hpp", -- slow compile times
		"%{prj.name}/vendor/glm/glm/**.inl",

		"%{prj.name}/vendor/stb_image/stb_image.cpp",

		"%{prj.name}/vendor/spirv-cross/**.hpp", 
		"%{prj.name}/vendor/spirv-cross/**.cpp", -- slow compile times. TODO: Get binaries
	}
	
	
	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/so-so/",

		"%{prj.name}/vendor/",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/glm",
		"%{prj.name}/vendor/stb_image",
		"%{prj.name}/vendor/assimp/include",

		"%{prj.name}/vendor/shaderc/include/libshaderc",
        "%{prj.name}/vendor/shaderc/include/libshaderc_util",
        "%{prj.name}/vendor/spirv-cross",
	}

	links 
	{
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib",
		"dwmapi.lib",
	}


	filter "system:windows"
		
		systemversion "latest"

		defines 
		{
			"SS_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

		
		libdirs 
		{
			"%{prj.location}/vendor/shaderc/bin/windows/%{cfg.buildcfg}",
		}

		links
		{
			"shaderc_combined.lib" -- Shaderc is fucking huge. Figure out how to reduce it's size
		}

	filter "system:linux"

		defines
		{
			"SS_PLATFORM_LINUX"
		}

	filter { "files:**/vendor/spirv-cross/**.cpp" }
    	flags { "NoPCH" }

	
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
	cppdialect "C++23"
	staticruntime "on"

	targetdir ("bin/" .. Outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. Outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

		"Resources/Shader/**.glsl"
	}

	includedirs
    {

    	"%{prj.name}/src",
    	
        "so-so/src",
        "so-so/vendor/spdlog/include",
        "so-so/vendor/GLFW/include",
        "so-so/vendor/Glad/include",
        "so-so/vendor/imgui",
        "so-so/vendor/glm",
        "so-so/vendor/stb_image",

        "%{prj.name}/vendor/shaderc/include/libshaderc",
        "%{prj.name}/vendor/shaderc/include/libshaderc_util",
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

		libdirs
		{
			"%{wks.location}/so-so/vendor/assimp/bin/windows/%{cfg.buildcfg}",
		}


	filter { "system:windows", "configurations:Debug" }


		links 
		{
			"assimp-vc143-mtd.lib"

		}

		postbuildcommands {
			'{COPY} "../so-so/vendor/assimp/bin/windows/Debug/assimp-vc143-mtd.dll" "%{cfg.targetdir}"',
		}
			

	filter { "system:windows", "configurations:Release" }

		links 
		{
			"assimp-vc143-mt.lib"
		}

		postbuildcommands {
			'{COPY} "../so-so/vendor/assimp/bin/windows/Release/assimp-vc143-mt.dll" "%{cfg.targetdir}"',
		}


	filter "system:linux"

		defines
		{
			"SS_PLATFORM_LINUX"
		}

		libdirs 
		{
			"so-so/vendor/assimp/bin/linux/%{cfg.buildcfg}"
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
