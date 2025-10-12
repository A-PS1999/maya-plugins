cmake_minimum_required (VERSION 3.27)

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(PROJECT_NAME BlendDeformer)
project(${PROJECT_NAME})

set(SOURCE_FILES 
	VoxelizerNode.h
	VoxelizerNode.cpp
	main.cpp
)

set(LIBRARIES OpenMaya Foundation)

build_plugin()
