cmake_minimum_required(VERSION 3.14)
project(openRange)

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

# Link libraries
if(APPLE)
    target_link_libraries(openRange
        SDL2
        SDL2_image
        "-framework OpenGL"
        "-framework GLUT"
    )
else()
    target_link_libraries(openRange
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${OPENGL_LIBRARIES}
    )
endif()

