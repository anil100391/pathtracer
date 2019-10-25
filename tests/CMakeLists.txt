cmake_minimum_required (VERSION 3.1)
project (pathtracer)

set (CMAKE_CXX_STANDARD 17)
# for msvc 2019 math macro definitions
add_definitions(-D_USE_MATH_DEFINES)
add_executable (vectests vec3Tests.cpp)
