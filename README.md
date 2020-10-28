# Simple 3D scene building game / Minecraft look-alike

An OpenGL-GLUT 3D Game. The scene consists of an imaginary 3D grid (X, Y, Z) of dimensions NxNxN, where in each position of the grid a small cube of a certain color can be placed. The positions of the grid are from 1 to N in each dimension. N is defined by user during runtime. The player is initially placed on a floor paved with cubes of different colors and moves on the floor by building, disappearing, going up, going down or falling. Game starts with player having 50 points and 3 lives and lasts as long as the player has remaining lives. Every time points go below zero, player loses 1 life. The aim for the player is to collect as many points as possible. Once player reaches the max level (N) in Z dimension, 100 points are added to his stock, game ends and he is proclaimed a winner.

## Game controls
The player can move back and forth with the "W" and "S" buttons or turn left and right without changing the square with the "A", "D" buttons respectively. These functions are done slowly, so that the player does not lose his orientation. The movement is possible when there is no cube at the same level (Z dimension) with the player in front of him, but there is a cube below that level, representing the floor he stands. If there is no floor below the player (no cube at Z-1 level) he falls by one or more levels by losing 5 points for each level fall besides the first one (ie loses 0 points for 1 level fall, 5 for 2, 10 for 3, etc). If the player falls into the void or does not have enough points he loses one life and gets respawned at magenta cube, which is the game reference cube, placed at grid center.

* Spacebar: Increases the stock of virtual cubes player has by one, while at the same time he loses 5 points. The number of virtual cubes available depends on the color of the cube on which the player sits (1 for yellow, 2 for red, 3 for green and 0 for blue and magenta). The cube below changes color to indicate his stock after the player gets one or more cubes.

* Q: The cube in front of the player being at the same level will disappear. When a cube disappears, the upper levels are not affected. 

* E: All cubes in front of him that are in the (X, Y) position and in any level (Z) will disappear. In this way he gains a life but loses 20 points.

* R: All the cubes begin to fall to lower levels until they meet another cube. If not, they disappear. This operation is done in steps to graphically represent the collapse. From this mode the player gets 10 points

* Left click: Places a random cube of players stock in front of him at the same level. Doing so will reduce his stock by 1 while giving him 5 points. When there is another cube on the same level in front of it, the cube will be added to the highest free level.

* Right click: The player can "kick" the cube in front of him at the same level. That cube will be moved by one position towards at where the player is looking. If the moving cube meets another one in front of it, then it also moves forward, while if a cube comes out of the grid due to the above chain reaction, it disappears.

* F1: Camera changes from 1st to 3rd person view and follows the player who is now represented by an anthropomorphic avatar, whose movement simulates human movement (ie the limbs of the avatar body are moving as he moves). The above is done when the player moves on the stage in any X, Y position at the same Z level. If during movement, the Z level is greater than the current level, the avatar is "teleported" to the new position.

* F2: Activates a spotlight that avatar carries, which follows its orientation and movements by illuminating the objects in front of it. (Provided that camera is in the 3rd person and the avatar is being displayed)

* ESC: Terminates the game while playing, and exits the app while in "Game Over" status.

## Required libraries
```
OpenGL – GLUT
```

## How to compile and run
```
Use the provided 'Makefile' to compile the game and create an executable.
Usage: "make PROG=Game"

After successful compilation, run the executable on terminal by giving the grid dimension as a parameter.
Example: "./Game 10"
```