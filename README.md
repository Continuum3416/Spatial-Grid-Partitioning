# Spatial Grid Partitioning Collision Detection

Overview: this is an implementation of spatial grid partitioning for more optimized collision detection. I also improved rendering speed by minimizing object creation, avoiding wrapper classes, and reducing draw calls.

Minimum Requirements: 
1. C++17
2. [CMake](https://cmake.org/) 3.10  
3. Have [SFML](https://www.sfml-dev.org/download/) 2.6.2 installed on your computer. Do not use SFML 3.0.0 as they changed a lot of stuff and remember to modify the path to your SFML library in CMakeLists.txt


## Preview

The code (on my machine) can handle up to 21,000 objects at over 60 FPS and 23,000 objects at over 50 FPS in release mode. Illustration in the demo below:

[comment]: <> (Hosted with git)
<p align="center">
<img alt="GIF" src="https://github.com/Continuum3416/git-media/blob/main/gif/23000-seg1.gif" width="600">
</p>

## Build

Create a `build` directory

```bash
mkdir build
cd build
```

**Configure**, **build**, and **run** the project with **release** (recommended)

```bash
cmake --build .\build\ --config Release
.\build\Release\grid
```

or with **debug**:

```bash
cmake --build .\build\ --config Debug
.\build\Debug\grid
```

Same thing for window, but remember to run with .exe instead.

## Settings:

To enable gravity, go to `headers/Verlet.h` and change

```c++
sf::Vector2f acceleration      = {0.f, 0.f};
```

to

```
sf::Vector2f acceleration      = {0.f, earth::g_f * SCALE};
```

To shoot some balls, go to main (grid.cpp) and uncomment the two lines

```c++
//handle_event.dragAndShoot<VerletBall>(event, solver);
//renderer.renderDragArrow(handle_event);
```

## Note:

There are still plenty of optimizations and physics corrections to be made, particularly when a large number of objects are stacked on top of each other with gravity enabled.