# The actual game
if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

set(SRC_DIR src)
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