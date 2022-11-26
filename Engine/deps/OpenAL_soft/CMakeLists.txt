add_library(OpenAL_Soft INTERFACE
        include/AL/al.h
        include/AL/alc.h
        include/AL/alext.h
        include/AL/efx.h
        include/AL/efx-creative.h
        include/AL/efx-presets.h
)

target_include_directories(OpenAL_Soft INTERFACE include)
set_target_properties(OpenAL_Soft PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(OpenAL_Soft INTERFACE OpenAL32)
target_link_directories(OpenAL_Soft INTERFACE "libs/Win64")