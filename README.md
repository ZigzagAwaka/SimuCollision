# SimuCollision
Solar system collision simulation, not scientific, but is fun to watch.

Collisions are detected using geometric algebra.

## **Installation**
Compilation is possible using Cmake, both on Windows and Linux.

### Linux:
```
> mkdir build
> cd build
> cmake ..
> make
```

### Windows:
Compilation can be done using command line (similar to Linux), but it's recommended to setup a VSCode c++ compilator and use the integrated options. Here is the steps in VSCode to compile this program:
```
> Go to the CMake Configure tab, choose a compilator and the "Release" option. Select "SimuCollision" as the Build and Launch targets.
It should launch cmake automatically.

> At the bottom of Vscode, choose the Build option, and it should compile the program.
Please note that warnings are expected because some external libraries are using old programming methods.

> At the bottom of Vscode, choose the Launch option, and it should open the program main window.
```

## **Usage**
The executable should be located in a new directory called bin/.

You can move the camera by clicking on the screen while moving your cursor and scroll to zoom.

You can show the golbal hitbox by pressing to O key, or modify the draw methods with the F, L and P keys.

If using the freefly camera, you can use the Z, Q, S and D keys to move (or WASD if English keyboard).

## **Execution**
![Screenshot](doc/sc.PNG?raw=true "Screenshot")

When you launch the program, 5 planets will randomly spawn in the space. They are located in a golbal hitbox of size 100. All planets have a random texture, size, rotation speed, inclination, and they all move in a random direction.
If a planet happens to touch the hitbox, they will go to the opposite direction, and they can also change direction randomly during the simulation.

If 2 planets collides they will explodes and create a random number of other smaller planets. Additionally, the collision will also generate small white particles to show the impact of the collision (these particles are not going to stay very long).

When a new planet spawns, there is a 1.5 seconds timer of invincibility period during which the planet can't collide with anything.

You can pause the time by pressing the spacebar. Accelerate or descelerate it with the + or - keys (on the keypad).

If the simulation starts to getting laggy, you can delete all small planets by pressing the 1 key on the keypad. You can also spawn a new big planet with the 0 key.

## **Project directories**
Textures are stored in assets/textures/ and they are copied automatically to the bin/assets/textures/ during compilation.

Source files are located in the src/ folder. I also added personal source files in the glimac/src/ folder since they all use the template of the glimac library (Circle, FreeflyCamera and TrackballCamera).

This project integrates the C3GA library and it's Eigen3 dependency manually. They are located in the lib/ folder.

## **Authors and acknowledgment**
By Ledoux Johan

Special thanks to Nozick Vincent
