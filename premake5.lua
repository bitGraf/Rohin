workspace "Rohin"
    architecture  "x86_64"
    startproject "Game"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture }"

-- Include directories relative to root folder
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Engine/deps/GLFW/include"
IncludeDir["GLAD"] = "%{wks.location}/Engine/deps/GLAD/include"
IncludeDir["ImGui"] = "%{wks.location}/Engine/deps/imgui"

include "Engine/deps/GLFW"
include "Engine/deps/GLAD"
include "Engine/deps/imgui"

project "Engine"
    location "Engine"
    kind "StaticLib"
    
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-intermediate/" .. outputdir .. "/%{prj.name}")

    pchheader "enpch.hpp"
    pchsource "Engine/src/enpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/deps/stb/**.cpp",
        "%{prj.name}/deps/stb/**.h"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/deps/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.ImGui}",
        "%{prj.name}/deps/picojson",
        "%{prj.name}/deps/stb"
    }

    links
    {
        "GLFW",
        "GLAD",
        "ImGui",
        "opengl32.lib"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "ROHIN_ENGINE"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GLFW_INCLUDE_NONE"
        }
    
    filter "configurations:Debug"
        defines "RH_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "RH_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "RH_DIST"
        runtime "Release"
        optimize "on"


project "Game"
    location "Game"
    kind "ConsoleApp"
    
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-intermediate/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Engine/deps/spdlog/include",
        "Engine/deps/picojson",
        "Engine/src",
        "Engine/deps/"
    }

    links
    {
        "Engine"
    }

    defines {
        "ROHIN_GAME"
    }

    filter "system:windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        defines "RH_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "RH_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "RH_DIST"
        runtime "Release"
        optimize "on"