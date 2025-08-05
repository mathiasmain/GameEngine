# Goal
For now, I just want to get a basic understanding of the Vulkan API and display something on the screen. It's a work in progress. 

# How to run
``` c++
mkdir build
cd build
cmake .. -DSDL_STATIC=ON -DSDL_SHARED=OFF
cmake --build .
```

You will need Vulkan installed, SDL3 and GLM git repositories in a thirdparty directory ( you can change this to your preferences in the CMakeLists.txt file ) at the root of the project.

Tested on Windows 11.

Based in:
- https://vulkan-tutorial.com
- https://github.com/stevelittlefish/c_vulkan_sdl3/blob/main/src/main.c
