workspace "FlyAway"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    system "Windows"

project "FlyAway"
    kind "ConsoleApp"
    language "C++"
    location "FlyAway"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    objdir "bin-int/%{cfg.buildcfg}/%{prj.name}"

    includedirs { "vendor/glew/include", "vendor/glfw/include" }
    libdirs { "vendor/glew/lib", "vendor/glfw/lib" }

    defines { "GLEW_STATIC" }

    links { "glfw3", "glew32s", "opengl32" }

    files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"
        symbols "Off"
        postbuildcommands {
            "{COPY} shaders/ ../bin/%{cfg.buildcfg}/%{prj.name}/shaders",
            "{COPY} models/ ../bin/%{cfg.buildcfg}/%{prj.name}/models", 
        }    