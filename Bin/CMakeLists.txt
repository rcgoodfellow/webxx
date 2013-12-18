include_directories(${CMAKE_SOURCE_DIR}/Lib)

add_executable(webxx driver.cc)
target_link_libraries(webxx web++)
