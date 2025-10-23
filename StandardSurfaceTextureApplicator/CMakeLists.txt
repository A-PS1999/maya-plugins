cmake_minimum_required (VERSION 3.27)

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(PROJECT_NAME StandardSurfaceTextureApplicator)
project(${PROJECT_NAME})

set(SOURCE_FILES 
	StandardSurfaceTextureApplicatorCmd.h
	StandardSurfaceTextureApplicatorCmd.cpp
	PluginMain.cpp
)

set(LIBRARIES OpenMaya Foundation)

build_plugin()
