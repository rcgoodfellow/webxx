cmake_minimum_required(VERSION 2.8)
project(Web++)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS
    "-fcolor-diagnostics -std=c++11 -stdlib=libc++ -g -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Werror")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  include_directories(/usr/local/include 
                      /usr/local/include/c++/v1)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++abi -pthread")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_subdirectory(Lib)
add_subdirectory(Bin)
