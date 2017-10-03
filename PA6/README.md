# PA6: Assimp Texture Loading  

# Group members
Made with [Matt Johnson](https://github.com/antilectual/cs480Johnson)

# Dependencies, Building, and Running

## Dependency Instructions
For both of the operating systems to run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

The project also uses [ImGUI](https://github.com/ocornut/imgui) and [JSON for Modern C++](https://github.com/nlohmann/json), both of which are already included.

This project uses OpenGL 3.3. Some computers, such as virtual machines in the ECC, can not run this version. In in order to run OpenGL 2.7 follow the instructions at [Using OpenGL 2.7](https://github.com/HPC-Vis/computer-graphics/wiki/Using-OpenGL-2.7)

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev libassimp-dev magick++-dev
```

## Building and Running

### CMake Instructions
The building of the project is done using CMake, installation with apt-get or brew may be necessary. Later use with CMake and Shader files will be require the copy of a directory where those files are stored (ex. shaders). To do this in the ```add_custom_target``` function place

```bash
mkdir build
cd build
cmake ..
make
./Tutorial config.json
```

# Controls

`left arrow` - Slow down  
`right arrow` - Speed up  
`r` - Reverse  
`s` - Stop  
`esc` - Quit

Hold `shift` to only affect the circular movement  
Hold `ctrl` to only affect the spinning movement

`left click` - Drag to rotate camera around object  
`Scroll wheel` - Zoom in and out



# Menu Usage
![Menu](https://i.imgur.com/tI66ihc.png)

**Camera Distance** - Changes how far the camera is away from the current object  
**Camera Rotation** - Rotates the camera around whatever the camera is looking at. Useful for getting out of a planet's shadow  
**Realistic/Close Scale** - Switch between the two to get realistic planet scaling (where the sun is *much* bigger than the planets and everything is very far apart) or an easier to see scale

### Planet Controls

**Select Satellite** - Select which planet/moon the controls will affect  
**Time Scale** - How fast the currently selected object will orbit and spin, as well as everything orbiting it  
**Orbit Speed** - How fast the currently selected object orbits, as well as the direction  
**Spin Speed** - How fast the currently selected object spins, as well as the direction  
**Focus** - Will cause the camera to follow this object around. Whatever the object is currently orbiting will be in the background.

# Command Line Parameters

## Command Usage

`Tutorial --help` - Pull up the help menu / command usage  
`Tutorial <config>` - Run the program with the given config file
