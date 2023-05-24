## Ball, Obstacle, and Target Simulation

This repository contains a C++ code that simulates the movement of a ball towards a target while avoiding an obstacle. The simulation is implemented using the Win32 API for graphical representation.

The code defines three structures: `Ball`, `Target`, and `Obstacle`, representing the positions of the ball, target, and obstacle, respectively. The simulation calculates the cost (distance) between the ball and the target, and adjusts the ball's position based on the gradient descent algorithm.

The main simulation loop handles user interface events using the Win32 API. It continuously updates the ball's position, checks for collisions with the obstacle, and redraws the graphical representation on the window. The simulation also includes a reset mechanism when the ball reaches the target.

To compile and run this code, a Windows environment and a C++ compiler are required. The code utilizes the Win32 API functions for window creation, event handling, and drawing on the screen.
