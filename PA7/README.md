# PA7: Solar System 

# Group members
Made with [Matt Johnson](https://github.com/antilectual/cs480Johnson)

# Dependencies, Building, and Running

## Dependency Instructions
For both of the operating systems to run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

The project also uses [ImGUI](https://github.com/ocornut/imgui) and [JSON for Modern C++](https://github.com/nlohmann/json), both of which are already included.

This project uses OpenGL 3.3. Some computers, such as virtual machines in the ECC, can not run this version. In in order to run OpenGL 2.7 follow the instructions at [Using OpenGL 2.7](https://github.com/HPC-Vis/computer-graphics/wiki/Using-OpenGL-2.7)   
   
This project uses ImageMagick++ for loading textures. We have found that there are various forms of ImageMagick++ that can conflict with each other. In order to be sure that ImageMagick++ is installed properly for this project, we recommend removing all other forms before installing the libmagick++-dev library. 
``` sudo apt-get remove *magick* ``` should handle this removal.

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev libassimp-dev libmagick++-dev
```

## Building and Running

### CMake Instructions
The building of the project is done using CMake, installation with apt-get or brew may be necessary. Later use with CMake and Shader files will be require the copy of a directory where those files are stored (ex. shaders). To do this in the ```add_custom_target``` function place

```bash
mkdir build
cd build
cmake ..
make
./Tutorial
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

# Things to check out

### Earth's Nighttime textures
![Earth](https://i.imgur.com/wwPRcj1.png)

### The Moon's Orbit
![Moon](https://i.imgur.com/d84EAvg.png)

### The Sun's Layered Textures
![Sun](https://i.imgur.com/wOT7HgD.png)

# Menu Usage
![Menu](https://i.imgur.com/AIt1yMM.png)

**Camera Distance** - Changes how far the camera is away from the current object.  Zooms out to overhead view.
**Camera Rotation** - Rotates the camera around whatever the camera is looking at. Useful for getting out of a planet's shadow  
**Realistic/Close Scale** - Switch between the two to get realistic planet scaling (where the sun is *much* bigger than the planets and everything is very far apart) or an easier to see scale  
**Orbits** - Check appropriate box to draw dotted lines along the planets' or moons' orbits   
**Labels** - Check appropriate box to display names over planets or moons

### Planet Controls

Click on the header to open.

**Select Satellite** - Select which planet/moon the controls will affect  
**Time Scale** - How fast the currently selected object will orbit and spin, as well as everything orbiting it. 
**Orbit Speed** - How fast the currently selected object orbits, as well as the direction  
**Spin Speed** - How fast the currently selected object spins, as well as the direction 

# Command Line Parameters

## Command Usage

`Tutorial` - Will run using the default configuration of `config.json`.   
`Tutorial --help` - Pull up the help menu / command usage  
`Tutorial <config>` - Run the program with the given config file (e.g. "Tutorial config.json")     