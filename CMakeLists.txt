cmake_minimum_required(VERSION 3.15)
project(MyRaylibVideoTemplate LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 可选：让 Raylib 使用 OpenGL ES（移动端常用）
# set(GRAPHICS_API "GRAPHICS_API_OPENGL_ES2")

option(BUILD_EXAMPLES "Build raylib examples" OFF)
option(BUILD_GAMES    "Build raylib games" OFF)

add_subdirectory(libs/raylib)

add_executable(main
    src/main.c
)
target_link_libraries(main PRIVATE raylib)

