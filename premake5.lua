include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Runa-SDL"
	architecture "x86_64"
	startproject "Runa"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
group ""

include "Runa"
