Jordan Cazamias\
CS348C - Animation & Simulation\
Nov. 18 2016

# Submission

This is my implementation of the Position Based Dynamics cloth.  This project was completed using OpenFrameworks.  The source code is included and can be found at https://github.com/jaycaz/ClothSimulation.

Executables can be built in Windows (using Visual Studio 2015), Mac, or Linux.  A Windows executable is also included for convenience, in the bin folder.

Here are the steps I completed for the Position Based Cloth Simulation:
 
- Cloth affected by gravity
- CollisionPlane objects can be added to the scene, which the cloth will not cross
- Cloth respects stretch and bend constraints
- User can pick a point on the cloth mesh and move it at will
- User can fix a point on the cloth mesh at a particular point in space
- Velocity damping to dissipate non-rigidbody energy
 
# Usage

Use the left mouse button to drag points of the cloth around and release to let go of the cloth.  

Use the right mouse button to drag points of the cloth around and release to fix that point in space.

Use the Ctrl key plus Left/Right Mouse Button to navigate.

