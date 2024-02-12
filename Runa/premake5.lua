project "Runa"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	-- save this in a variable: "%{wks.location}/bin/" .. outputdir .. "/%{prj.name}"
	-- then use it in the postbuildcommands

	pchheader "runapch.h"
	pchsource "src/runapch.cpp"

	files {"src/**.h", "src/**.cpp"}

	defines {"_CRT_SECURE_NO_WARNINGS", "GLFW_INCLUDE_NONE"}

	includedirs {
		"%{wks.location}/Runa/vendor/spdlog/include", 
		"%{wks.location}/Runa/src",
		"%{wks.location}/Runa/vendor",
		"%{IncludeDir.SDL2}",
		"%{IncludeDir.SDL2_image}",
		"%{IncludeDir.SDL2_ttf}"
	}

	links {"SDL2", "SDL2main", "SDL2_image", "SDL2_ttf"}

	libdirs {"%{LibraryDir.SDL2}", "%{LibraryDir.SDL2_image}", "%{LibraryDir.SDL2_ttf}"}

	debugenvs {"PATH=%{LibraryDir.SDL2};%{LibraryDir.SDL2_image};%{LibraryDir.SDL2_ttf}"}

	filter "system:windows"
		systemversion "latest"

		links {"SDL2", "SDL2main", "SDL2_image", "SDL2_ttf"}
		postbuildcommands {"{COPY} \"%{wks.location}Runa\\vendor\\SDL2\\lib\\x64\\SDL2.dll\" \"%{wks.location}bin\\" .. outputdir .. "\\%{prj.name}\""}
		postbuildcommands {"{COPY} \"%{wks.location}Runa\\vendor\\SDL2_image\\lib\\x64\\SDL2_image.dll\" \"%{wks.location}bin\\" .. outputdir .. "\\%{prj.name}\""}
		postbuildcommands {"{COPY} \"%{wks.location}Runa\\vendor\\SDL2_ttf\\lib\\x64\\SDL2_ttf.dll\" \"%{wks.location}bin\\" .. outputdir .. "\\%{prj.name}\""}


	filter "configurations:Debug"
		defines "RUNA_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RUNA_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RUNA_DIST"
		runtime "Release"
		optimize "on"


