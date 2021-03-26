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
IncludeDir["OpenAL_soft"] = "%{wks.location}/Engine/deps/OpenAL_soft/include"

include "Engine/deps/GLFW"
include "Engine/deps/GLAD"
include "Engine/deps/imgui"
include "Engine/deps/OpenAL_soft"

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
        "%{IncludeDir.OpenAL_soft}",
        "%{prj.name}/deps/stb"
    }

    links
    {
        "GLFW",
        "GLAD",
        "ImGui",
        --"OpenAL_soft",
        "opengl32.lib"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "ROHIN_ENGINE"
    }

    -- Clean function --
    if _ACTION == "clean" then
        os.remove("Engine/Engine.vcxproj")
        os.remove("Engine/Engine.vcxproj.filters")
        os.remove("Engine/Engine.vcxproj.user")
     end

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GLFW_INCLUDE_NONE"
        }
    
    filter "configurations:Debug"
        defines { "RH_DEBUG", "_DEBUG" }
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines { "RH_RELEASE", "NDEBUG" }
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines { "RH_DIST", "NDEBUG" }
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
        "Engine/src",
        "Engine/deps/"
    }

    links
    {
        "Engine",
        "OpenAL32"
    }

    libdirs
    {
        "%{wks.location}/Engine/deps/OpenAL_soft/libs/Win64"
    }

    postbuildcommands {
        "{COPY} %{cfg.targetdir}/Game.exe %{wks.location}/Game/"
    }

    defines {
        "ROHIN_GAME"
    }

    -- Clean function --
    if _ACTION == "clean" then
        os.remove("Game/Game.vcxproj")
        os.remove("Game/Game.vcxproj.filters")
        os.remove("Game/Game.vcxproj.user")
     end

    filter "system:windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        defines { "RH_DEBUG", "_DEBUG" }
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines { "RH_RELEASE", "NDEBUG" }
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines { "RH_DIST", "NDEBUG" }
        runtime "Release"
        optimize "on"

-- Clean Function --
newaction {
    trigger     = "clean",
    description = "clean the software build files",
    execute     = function ()
       print("clean the build...")
       os.rmdir("./bin")
       os.rmdir("./bin-intermediate")
       os.remove("./Rohin.sln")
       print("done.")
    end
 }