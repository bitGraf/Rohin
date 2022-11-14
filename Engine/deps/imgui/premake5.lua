project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imgui/imconfig.h",
		"imgui/imgui.h",
		"imgui/imgui.cpp",
		"imgui/imgui_demo.cpp",
		"imgui/imgui_draw.cpp",
		"imgui/imgui_widgets.cpp",
		"imgui/imgui_tables.cpp",
		"imgui/imgui_internal.h",
		"imgui/imstb_rectpack.h",
		"imgui/imstb_textedit.h",
		"imgui/imstb_truetype.h",
		"imgui/backends/imgui_impl_opengl3.cpp",
		"imgui/backends/imgui_impl_glfw.cpp"
	}

	includedirs
    {
        "imgui",
		"%{wks.location}/Engine/deps/GLFW/GLFW/include"
    }

	links
    {
        "GLFW",
        "GLAD",
    }

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"