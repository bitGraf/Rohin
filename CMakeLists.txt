cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT RohinApp)

#set (CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/CompilerOptions.cmake")
message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

# Set the project name and language
project(
  Rohin
  VERSION 0.0.1
  DESCRIPTION ""
  HOMEPAGE_URL ""
  LANGUAGES CXX C)

# put them all in the same folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin) #.lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin) #.dll
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin) #.exe

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)

add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(RohinApp)

# build timing
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../ctime/ctime.exe")
    message(STATUS "Found CTime")
    
    add_custom_target(start_time ALL
        COMMAND ../ctime/ctime.exe -begin Tools/build_time_cmake.ctm
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
        COMMENT "Starting build timing"
        )
    set_target_properties(start_time PROPERTIES FOLDER "utils")

    add_custom_target(end_time ALL
        COMMAND ../ctime/ctime.exe -end Tools/build_time_cmake.ctm 0 # NOT RECORDING ERROR CODE!
        DEPENDS rohin game RohinApp
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
        COMMENT "Ending build timing"
        )
    set_target_properties(end_time PROPERTIES FOLDER "utils")
endif()