# 2d Physics

A small 2D Physics engine developed for learning more about collision detection and collision response. It's written in C++ using [SDL2](https://www.libsdl.org/) and [imgui](https://github.com/ocornut/imgui).

## Build Instructions

- Requirements
  - Git
  - CMake
  - C++ Compiler

- Steps

    ```cmd
    # Clone this project along with submodules
    $ git clone --recursive https://github.com/AbhayJoshi-Dev/2DPhysics

    # Go to project directory
    $ cd 2DPhysics

    # Pull SDL as submodule (if you cloned without --recursive flag)
    $ git submodule update --init

    # Create a folder for build
    $ mkdir build

    # Go to build folder
    $ cd build

    # Generate project files
    $ cmake ..
    ```

- This will generate project files using default cmake generator for your platform. For example:

  - on `Windows` with `Visual Studio` installed, it will generate a `.sln`
  - on `Linux` with `make` installed, it will generate a `Makefile`
  - on `Mac` with `xcode` installed, it will generate a `.xcodeproj`