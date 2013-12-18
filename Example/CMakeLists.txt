include_directories(${CMAKE_SOURCE_DIR}/Lib)

add_executable(example0 example0.cc)
target_link_libraries(example0 web++)
