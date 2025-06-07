workspace "so-so"

	startproject "Sandbox"
	configurations { "Debug", "Release", "Dist" }
	conformancemode "On"

	staticruntime "Off"
	language "C++"
	cppdialect "C++23"

	flags { "MultiProcessorCompile" }

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"NOMINMAX",
		"SPDLOG_USE_STD_FORMAT",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"
	}

	filter "action:vs*"
        linkoptions { "/ignore:4099" } -- Disable no PDB found warning

	filter "language:C++ or language:C"
		architecture "x86_64"
	

--==================================================================================================================

Outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "so-so/vendor/GLFW"
include "so-so/vendor/Glad"
include "so-so/vendor/imgui"

project "so-so"
	kind "StaticLib"
	location "so-so"
	staticruntime "Off"

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

		"%{prj.name}/vendor/stb_image/stb_image.cpp",

		"%{prj.name}/vendor/spirv-cross/**.hpp", 
		"%{prj.name}/vendor/spirv-cross/**.cpp", 
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

		"%{prj.name}/vendor/shaderc/include",
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

--==================================================================================================================

project "Sandbox"
	kind "ConsoleApp"
	location "Sandbox"

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
			"%{wks.location}/so-so/vendor/assimp/lib/windows/%{cfg.buildcfg}",
			"%{wks.location}/so-so/vendor/shaderc/lib/windows/%{cfg.buildcfg}",
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


	filter { "system:windows", "configurations:Debug" }

		links 
		{
			"assimp-vc143-MTd.lib",
			"shaderc-shared-MDd.lib"
		}

		postbuildcommands {
			'{COPY} "../so-so/vendor/assimp/lib/windows/Debug/assimp-vc143-MTd.dll" "%{cfg.targetdir}"',
			'{COPY} "../so-so/vendor/shaderc/lib/windows/Debug/shaderc-shared-MDd.dll" "%{cfg.targetdir}"',
		}
			

	filter { "system:windows", "configurations:Release" }

		links 
		{
			"assimp-vc143-MT.lib",
			"shaderc-shared-MD.lib"
		}

		postbuildcommands {
			'{COPY} "../so-so/vendor/assimp/lib/windows/Release/assimp-vc143-MT.dll" "%{cfg.targetdir}"',
			'{COPY} "../so-so/vendor/shaderc/lib/windows/Release/shaderc-shared-MD.dll" "%{cfg.targetdir}"',
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
