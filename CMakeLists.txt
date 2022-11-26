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

# Subdirectories
add_subdirectory(Engine)

# The actual game
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

set(SRC_DIR Game/src)
set(GAME_SRC
    ${SRC_DIR}/main.cpp
)
set(GAME_SCENES_SRC
    ${SRC_DIR}/Scenes/Level.hpp
    ${SRC_DIR}/Scenes/Level.cpp
    ${SRC_DIR}/Scenes/MainMenu.hpp
    ${SRC_DIR}/Scenes/MainMenu.cpp
)
set(GAME_SCRIPTS_SRC
    ${SRC_DIR}/Scripts/Gem.hpp
)
set(ALL_SRC
    ${GAME_SRC}
    ${GAME_SCENES_SRC}
    ${GAME_SCRIPTS_SRC}
)
add_executable( Game ${ALL_SRC})
target_link_libraries(Game PUBLIC Engine)
target_include_directories(Game PRIVATE ${SRC_DIR})
source_group(src FILES ${GAME_SRC})
source_group(src\\Scenes FILES ${GAME_SCENES_SRC})
source_group(src\\Scripts FILES ${GAME_SCRIPTS_SRC})
set_target_properties(Game PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/Game/run_tree"
                        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/Game/run_tree")
set_property(TARGET Game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Game/run_tree")