cmake_minimum_required(VERSION 3.14)
project(openRange)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adjust this if you're on Intel (change /opt/homebrew to /usr/local)
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(openRange main.cpp)

target_link_libraries(openRange
    SDL2
    SDL2_image
    GLEW
    "-framework OpenGL"
)

