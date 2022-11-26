add_library(stb STATIC
        stb.h
        stb_image.h
        stb_implementation.cpp
        stb_truetype.h
        stb_vorbis.c
)

target_include_directories(stb PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)