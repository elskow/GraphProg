cmake_minimum_required(VERSION 3.29)
project(triangle)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)

add_executable(wglfw main_glfw.cpp)
add_executable(nolib main_nolib.cpp)

target_link_libraries(
        wglfw
        OpenGL::GL
        glfw
        glad::glad
)

target_link_libraries(
        nolib
        OpenGL::GL
        X11
)
