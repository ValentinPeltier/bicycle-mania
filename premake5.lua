require ".premake/ecc/ecc"

workspace "GameTest"
    configurations { "Debug", "Release" }
    architecture "x86_64"

project "GameTest"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    files { "src/**.hpp", "src/**.cpp" }
    includedirs { "src" }

    outputDir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg:lower()}"
    targetdir ("bin/" .. outputDir)
    objdir ("obj/" .. outputDir)

    filter "system:windows"
        systemversion "latest"
        links {
            "vulkan-1",
            "glfw3",
        }

    filter "system:linux"
        links {
            "vulkan",
            "glfw", "dl", "pthread", "X11", "Xxf86vm", "Xrandr", "Xi",
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"