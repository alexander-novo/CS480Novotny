# PA4: Planets w/ Sun Lighting

# Dependencies, Building, and Running

## Dependency Instructions
For both of the operating systems to run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

The project also uses [ImGUI](https://github.com/ocornut/imgui) and [JSON for Modern C++](https://github.com/nlohmann/json), both of which are already included.

This project uses OpenGL 3.3. Some computers, such as virtual machines in the ECC, can not run this version. In in order to run OpenGL 2.7 follow the instructions at [Using OpenGL 2.7](https://github.com/HPC-Vis/computer-graphics/wiki/Using-OpenGL-2.7)

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev
```

### Mac OSX
Installation of brew is suggested to easily install the libs. Ensure that the latest version of the Developer Tools is installed.
```bash
brew install glew glm sdl2
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
# Changing Models
![Config File](https://i.imgur.com/2DV9R2d.jpg)

To change the model of a planet, locate the planet in the config, and change the "model" parameter to the name of the .obj. The program will expect to find this model in the models/ directory.

If you're going to test this feature with the Stanford Dragon, please use the one included in the repository (Dragon.obj), as the one provided to us has broken materials and will fail to load (as well as has missing normals).

![Dragon](https://i.imgur.com/uEbMgTK.png)

*An example of a test performed by replacing one of the planets with the dragon.obj file*

# Controls

`up arrow` - Decrease orbit radius  
`down arrow` - Increase orbit radius  
`left arrow` - Slow down  
`right arrow` - Speed up  
`r` - Reverse  
`s` - Stop  
`esc` - Quit

`left click` - Reverse  
`right click` - Stop

Hold `shift` to only affect the circular movement  
Hold `ctrl` to only affect the spinning movement

# Menu Usage

![Menu](https://i.imgur.com/YeJxVZL.png)

**Camera Distance** - Changes how far the camera is away from the current object  
**Camera Rotation** - Rotates the camera around whatever the camera is looking at. Useful for getting out of a planet's shadow

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
