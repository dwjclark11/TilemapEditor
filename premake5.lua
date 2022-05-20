-- Currently Builds for Windows using Visual Studio 2019 Community!

workspace "JadeTilemapEditor"
	system "Windows"
	architecture "x86"

	configurations
	{
		"Debug",	-- All debug information
		"Release",	-- Still has logging
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "TilemapEditor"
	location "TilemapEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}" )
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}" )

	-- Included files and folders
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.inl",
		"%{prj.name}/libs/imgui/**.h",
		"%{prj.name}/libs/imgui/**.cpp",
	}

	-- VC++ Directories -- Include/Libraries
	sysincludedirs {"%{prj.name}/libs"}
	syslibdirs { "%{prj.name}/libs/lua" }

	-- C/C++: General - Additional Include Directories
	includedirs
	{
		"%{prj.name}/libs/SDL2/include",
        "%{prj.name}/libs/spdlog/include",
	}
	-- Linker: General - Additional Library Dependencies
	libdirs
	{
		"%{prj.name}/libs/SDL2/lib/x86",
	}

	-- This is for overriding the [.lib] for [.a] libraries
	premake.override(premake.tools.msc, "getLibraryExtensions", function(oldfn)
		local extensions = oldfn()
		extensions["a"] = true
		return extensions
	  end)
	-- Input: Addidtional Dependencies
	links
	{
		"SDL2",
		"SDL2main",
		"SDL2_image",
		"SDL2_ttf",
		"SDL2_mixer",
	}

	linkoptions {"liblua53.a"}

	-- Filters for different platforms
	filter "system:windows"
		systemversion "latest" 

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"WIN32",
		}

	filter "configurations:Debug"
		buildoptions "/MTd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		buildoptions "/MT"
		runtime "Release"
		optimize "full"


