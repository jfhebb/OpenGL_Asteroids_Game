COMP 3831 Demo Code
==========================

Requirements and Set-Up
--------------------------
- uses OpenGl 2.1 and GLSL 1.20
- requires glew and freeglut libraries on Windows 

Consult 'OpenGL Environment Set-Up' resources on moodle for set-up help.

Shader Problems
--------------------------
If you get compile errors for the shaders, it could be the line endings of the glsl
text files. They need to be Unix text files, not Windows text files (a bug in the
shader loading code). Use an editor that can save them as Unix encoded files (like
TextPad on Windows). I'll try to fix this in the future.

Switching Between Demo Sets
-----------------------------
The demos_* files cannot all be included and compiled together. Choose one at a
time to be compiled and included in the build and exclude the rest. This can be 
accomplished by copying and pasting code from demos_*.cpp into one file which is always
included in the build (like a 'demo.cpp' file). Or you can add and remove demos_*.cpp
from your project as you need them.

In Visual Studio, there is way to keep all 'demos_*.cpp' files in your project pane, but 
tell the compiler to ignore them: right-click the file, open file Properties, and set 
"Excluded from Build" in 'General' section to either 'Yes' or 'No'. 

Once you have it narrowed down to one demos_*.cpp file, set a preprocessor variable called
DEMO at the top to choose between different demos in the file

Index of Demos
-----------------------------

demos_1.cpp:

	DEMO 1: sierpinski gasket
	DEMO 2: circle
	DEMO 3: circle with colour

demos_2.cpp:

	DEMO 1: Triangle (2 buffers)
	DEMO 2: Triangle (1 buffer)
	DEMO 3: Fan TRIANGLES
	DEMO 4: Fan TRIANGLE_STRIP
	DEMO 5: Fan with Indices

demos_3.cpp:

	DEMO 1: animation
	DEMO 2: mouse line

demos_4.cpp:

	DEMO 1: pinhole perspective
	DEMO 2: pinhole perspective with shaders 
	DEMO 3: pinhole perspective with shaders using offset too
	DEMO 4: viewports

demos_5.cpp:

	DEMO 1: switch between VBOs, multiple instances, uniform colours 
	DEMO 2: transformations without matrices 
	DEMO 3: transformations with matrices
	DEMO 4: transformations in the shader (the best way)
	DEMO 5: rotation
	DEMO 6: concatenate transformations in 2D

demos_6.cpp:

	DEMO 1: 3D mesh class, wireframe offset, 3D transformations
	DEMO 2: consecutive transformations in 3D
	DEMO 3: saving intermediate state
	DEMO 4: table with cubes
	
demos_7.cpp:

	DEMO 1: move camera with rotation and translation
	DEMO 2: move camera using lookAt
	DEMO 3: using Ortho
	DEMO 4: with perspective
	
demos_8.cpp:

	DEMO 1: simple camera class, subdivided sphere mesh
	DEMO 2: same as DEMO 1, but showing depth buffer
	DEMO 3: fog using depth information
	DEMO 4: depth buffer limitation
	DEMO 5: back face culling with squares
	DEMO 6: cube with missing bottom

demos_9.cpp:

	DEMO 1: 2D line-line intersection

demos_10.cpp:

	DEMO 1: 2D morphing between 2 targets
	DEMO 2: 2D morphing between 3 targets
	DEMO 3: 2D morphing animation

demos_11.cpp:

	DEMO 1: hacked lighting: no normals, one colour, hard coded lighting param

demos_12.cpp:

	DEMO 1: lighting with normals using LitMesh, and light and material classes, and arcball


