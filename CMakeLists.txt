cmake_minimum_required (VERSION 3.1)
project (pathtracer)

set (CMAKE_CXX_STANDARD 17)
# for msvc 2019 math macro definitions
add_definitions(-D_USE_MATH_DEFINES)

add_subdirectory(tests)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

include_directories ("${PROJECT_SOURCE_DIR}/vendor/imgui")
include_directories ("${PROJECT_SOURCE_DIR}/vendor/stb_image")

# undefine min/max macro from msvc minwindef.h
add_compile_options(-DNOMINMAX)

# for definition of M_PI for msvc
add_compile_options(-D_USE_MATH_DEFINES)
add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

file (GLOB imgui_src "${PROJECT_SOURCE_DIR}/vendor/imgui/*.cpp" )
file (GLOB stb_img_src "${PROJECT_SOURCE_DIR}/vendor/stb_image/*.cpp" )

add_executable (tracer pathtracer.cpp ${imgui_src} ${stb_img_src})

if (MSVC)
    target_link_libraries (tracer PRIVATE glfw GLEW::GLEW opengl32)
else()
    target_link_libraries (tracer GL glfw GLEW)
endif()

