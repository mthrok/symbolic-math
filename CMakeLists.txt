cmake_minimum_required(VERSION 2.6.2)
project (Symbolic-Math-Test)

option(build_test "Build tests." ON)

# Enable c++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


include_directories("${PROJECT_SOURCE_DIR}/src/include")

if (build_test)
   add_subdirectory("${PROJECT_SOURCE_DIR}/test")
endif ()
