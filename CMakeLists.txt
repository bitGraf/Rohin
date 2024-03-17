cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)

add_subdirectory(Engine)
add_subdirectory(RohinGame)

# build timing
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../ctime/ctime.exe")
    message(STATUS "Found CTime")
    
    add_custom_target(start_time ALL
        COMMAND ../ctime/ctime.exe -begin Tools/build_time.ctm
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
        COMMENT "Starting build timing"
        )
    add_custom_target(end_time ALL
        COMMAND ../ctime/ctime.exe -end Tools/build_time.ctm 0 # NOT RECORDING ERROR CODE!
        DEPENDS rohin game
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/"
        COMMENT "Ending build timing"
        )
endif()