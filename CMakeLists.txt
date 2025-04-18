cmake_minimum_required(VERSION 3.10)
project(openRange)

# Enable compile commands generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Homebrew paths for macOS
if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Source files
set(SOURCES
    main.cpp
)

# Add executable
add_executable(openRange ${SOURCES})

# Include directories
target_include_directories(openRange PRIVATE 
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

# Link libraries
if(APPLE)
    target_link_libraries(openRange
        SDL2
        SDL2_image
        "-framework OpenGL"
        "-framework GLUT"
    )
else()
    target_link_libraries(openRange PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
endif()

