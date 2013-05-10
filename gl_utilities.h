#ifndef GL_UTILITIES_H
#define GL_UTILITIES_H

/*
	OpenGL Utilities

	Adapted from suplemental code provided by Interactive Computer Graphics
	A Top-Down Approach with OpenGL, Edward Angel and Dave Shreiner
	Sixth Edition, Addison-Wesley 2012
	http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/

	And sections from code supplied by http://openglbook.com/

	Portions may be:

	Copyright (C) 2011 by Eddy Luten

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

// Common system headers
#include <cmath>
#include <iostream>

#include "gl_include.h"



#include "vec.h"


// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace djv {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// returns a random float number between max and min
// negative values are permitted, as long as min < max
float randRange(float min = 0, float max = 1.0);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// convert an HSV (Hue, Saturation, and Value) colour to vec3 RGB 
// all values in range 0 - 1
vec3 hsv2rgb(float hue, float sat, float val);
vec4 hsv2rgb(float hue, float sat, float val, float alpha);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// clamp a value between min and max
inline float clamp(float val, float min, float max)
{
	return std::max(min, std::min(max, val));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

GLuint loadAndInitializeShaders(const char* vShaderFile, const char* fShaderFile);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// get attribute location with an error check
int glGetAttribLocationHelper(unsigned int program, char* name, bool exitOnError = false);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// get uniform location with an error check
int glGetUniformLocationHelper(unsigned int program, char* name, bool exitOnError = false);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// OpenGl error checking
static const char*
ErrorString( GLenum error )
{
    const char*  msg;
    switch( error ) {
#define Case( Token )  case Token: msg = #Token; break;
	Case( GL_NO_ERROR );
	Case( GL_INVALID_VALUE );
	Case( GL_INVALID_ENUM );
	Case( GL_INVALID_OPERATION );
	Case( GL_STACK_OVERFLOW );
	Case( GL_STACK_UNDERFLOW );
	Case( GL_OUT_OF_MEMORY );
#undef Case	
    }

    return msg;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void _CHECK_GL_ERROR( const char* file, int line )
{
    GLenum  error = glGetError();

    do {
	fprintf( stderr, "[%s:%d] %s\n", file, line, ErrorString(error) );
    } while ((error = glGetError()) != GL_NO_ERROR );
	
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define CHECK_GL_ERROR  _CHECK_GL_ERROR( __FILE__, __LINE__ )

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void checkGlError(const char* msg)
{
	GLenum ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: %s: %s \n",
			msg,
			gluErrorString(ErrorCheckValue)
		);

		exit(-1);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}

#endif