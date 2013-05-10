#ifndef GL_INCLUDE_H
#define GL_INCLUDE_H
/*
	OpenGL Include: A master include file

	Adapted from supplemental code provided by Interactive Computer Graphics
	A Top-Down Approach with OpenGL, Edward Angel and Dave Shreiner
	Sixth Edition, Addison-Wesley 2012
	http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/

*/

// Include OpenGL header files 
#ifdef __APPLE__  // include Mac OS X versions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__




// Define M_PI in case it's not defined in the math header file
#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

//  Degrees-to-radians constant 
#ifndef DegreesToRadians
#define DegreesToRadians GLfloat(M_PI / 180.0)
#endif



#endif
