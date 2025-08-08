require "vendor/premake-ecc/ecc"

workspace "Bicycle_Mania"
    configurations { "Debug", "Release" }
    architecture "x86_64"

project "Bicycle_Mania"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    files { "src/**.hpp", "src/**.cpp" }
    includedirs {
        "src",
        "vendor/glm",
    }

    outputDir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg:lower()}"
    targetdir ("bin/" .. outputDir)
    objdir ("obj/" .. outputDir)

    filter "system:windows"
        defines { "OS_WINDOWS" }
        systemversion "latest"
        links {
            "vulkan-1",
            "glfw3",
        }
    filter {}

    filter "system:linux"
        defines { "OS_LINUX" }
        toolset "clang"
        links {
            "vulkan",
            "glfw", "dl", "pthread", "X11", "Xxf86vm", "Xrandr", "Xi",
        }
        postbuildcommands ("./compile_shaders.sh")
    filter {}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    filter {}

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "On"
    filter {}

    postbuildcommands ("{COPYDIR} share bin/" .. outputDir)