add_library(EnTT INTERFACE src/ )

target_include_directories(
  EnTT INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(EnTT INTERFACE cxx_std_17)