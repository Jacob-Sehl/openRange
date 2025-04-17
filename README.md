# OpenRange

A 2D ranching game built with C++, SDL2, and OpenGL.

## Description

OpenRange is a simple 2D game where you control a ranger in an open world environment. The game features smooth movement and pixel-art graphics rendered using OpenGL.

## Prerequisites

To build and run this game, you'll need:

- CMake (3.14 or higher)
- C++ compiler with C++17 support
- SDL2
- SDL2_image
- GLEW
- OpenGL

### macOS Dependencies

Using Homebrew:
```bash
brew install sdl2 sdl2_image glew cmake
```

### Linux Dependencies

Using apt (Ubuntu/Debian):
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libglew-dev cmake
```

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/openRange.git
cd openRange
```

2. Create and enter the build directory:
```bash
mkdir build
cd build
```

3. Generate build files and compile:
```bash
cmake ..
make
```

## Running the Game

From the build directory:
```bash
./openRange
```

## Controls

- W - Move up
- S - Move down
- A - Move left
- D - Move right

## Project Structure

- `main.cpp` - Core game logic and rendering
- `assets/` - Game assets (sprites, textures)
- `shaders/` - GLSL shader files
- `CMakeLists.txt` - Build configuration

## Technical Details

The game is built using:
- SDL2 for window management and input handling
- OpenGL for rendering
- GLEW for OpenGL extension loading
- SDL2_image for texture loading

The game features:
- Smooth character movement
- Camera following system
- Basic texture loading and rendering

## Contributing

Feel free to fork the project and submit pull requests for any improvements you'd like to add.
