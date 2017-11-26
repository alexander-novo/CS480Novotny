# PA10: Pinball   

# Group members
[Matt Johnson](https://github.com/antilectual/cs480Johnson)   
[Alex Novotny](https://github.com/alexander-novo/cs480Novotny)

# Dependencies, Building, and Running

## Dependency Instructions
For both of the operating systems to run this project installation of these three programs are required [GLEW](http://glew.sourceforge.net/), [GLM](http://glm.g-truc.net/0.9.7/index.html), and [SDL2](https://wiki.libsdl.org/Tutorials).

The project also uses [ImGUI](https://github.com/ocornut/imgui) and [JSON for Modern C++](https://github.com/nlohmann/json), both of which are already included.

This project uses OpenGL 3.3. Some computers, such as virtual machines in the ECC, can not run this version. In in order to run OpenGL 2.7 follow the instructions at [Using OpenGL 2.7](https://github.com/HPC-Vis/computer-graphics/wiki/Using-OpenGL-2.7)   
   
This project uses ImageMagick++ for loading textures. We have found that there are various forms of ImageMagick++ that can conflict with each other. In order to be sure that ImageMagick++ is installed properly for this project, we recommend removing all other forms before installing the libmagick++-dev library. 
``` sudo apt-get remove *magick* ``` should handle this removal.

This project uses [Bullet Physics](https://bulletphysics.org).

### Ubuntu/Linux
```bash
sudo apt-get install libglew-dev libglm-dev libsdl2-dev libsdl2-mixer-dev libassimp-dev libmagick++-dev libbullet-dev
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

`left click` - Drag to rotate camera, click object to shoot object forward some     
`right click` - Shoot a cube forward, and several spheres in random directions.   
`Scroll wheel` - Zoom in and out  
`R` - Switch between per-vertex / per-fragment lighting  
`up/down arrows` - Widen/narrow spotlight   
`Spacebar` - Press to launch ball. Hold to increase power.     
`N` - To start a new game.   
`P` - To pause the game.   

# Menu

Pauses while 

### Game Options

`Plunger Type` - Choose between three styles of plunger: `Hold` space to charge, release to launch. `Static` predefined charge level, just push to launch. `Toggle` charge by hitting space, then hit space again to launch.   
`Play Style` - Choose to play with 1 ball at a time, or with 3 balls at once!   
`Ambient Lighting` - (HUGE menu option - Adjust options window size accordingly if needed) Adjust color and intensity of ambient lighting.   

### Graphics Options

`Shader Type` - Select Between Vertex and Fragment shading.
`Shadows` - Select shadow quality.  

# Command Line Parameters

## Command Usage

`Tutorial` - Will run using the default configuration of `config.json`.   
`Tutorial --help` - Pull up the help menu / command usage   
`Tutorial <config>` - Run the program with the given config file (e.g. "Tutorial config.json")   

# Extra Credit

Multiple Balls   
Plunger Intensity   
Spotlight that follows ball   
Shadow Mapping (shadows)  
Game Replay   
Bumper Lights   
Sound Effects / Music   
Pause Game