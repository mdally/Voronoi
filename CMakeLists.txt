cmake_minimum_required(VERSION 2.8)

project(Voronoi)

file(GLOB LIB_SOURCE "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} "include")
add_library(Voronoi ${LIB_SOURCE})

add_executable(Console_Example "examples/Console_Example.cpp")
add_executable(OpenGL_Example "examples/OpenGL_Example.cpp")

target_link_libraries(Console_Example 
debug debug/voronoi optimized release/voronoi)

target_link_libraries(OpenGL_Example 
debug lib/glew32_d optimized lib/glew32_r
debug lib/glfw3_d optimized lib/glfw3_r
debug debug/voronoi optimized release/voronoi)