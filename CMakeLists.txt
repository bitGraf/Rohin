cmake_minimum_required( VERSION 3.20 )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

project(
  "Rohin" 
  VERSION 0.1.0
  DESCRIPTION "a 3D Game Engine project."
  HOMEPAGE_URL "https://github.com/bitGraf/rohin"
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_TOPLEVEL_PROJECT TRUE)
else()
  set(IS_TOPLEVEL_PROJECT FALSE)
endif()

# Compile-time Switches
set(ROHIN_PLATFORM "Win32_64")
set(ROHIN_RENDERER "OpenGL")

# Subdirectories
add_subdirectory(Engine)
add_subdirectory(Game)