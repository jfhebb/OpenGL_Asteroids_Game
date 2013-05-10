/*
	OpenGL Utilities

	Adapted from suplemental code provided by Interactive Computer Graphics
	A Top-Down Approach with OpenGL, Edward Angel and Dave Shreiner
	Sixth Edition, Addison-Wesley 2012
	http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/

	And sections from code supplied by http://openglbook.com/

	Portions are:

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

#include "gl_utilities.h"

namespace djv {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
// returns a random float number between max and min
// negative values are permitted, as long as min < max
float randRange(float min, float max)
{
	return ((max - min) * (float)rand()/RAND_MAX) + min;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// convert an HSV (Hue, Saturation, and Value) colour to vec3 RGB 
// all values in range 0 - 1
vec3 hsv2rgb(float hue, float sat, float val)
{
	float v;
	float r,g,b;

	r = val;   // default to gray
	g = val;
	b = val;
	v = (val <= 0.5f) ? (val * (1.0f + sat)) : (val + sat - val * sat);

	if (v > 0)
	{
			float m;
			float sv;
			int sextant;
			float fract, vsf, mid1, mid2;

			m = val + val - v;
			sv = (v - m ) / v;
			hue *= 6.0f;

			sextant = (int)hue;
			fract = hue - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;

			switch (sextant)
			{
				case 0:
						r = v;
						g = mid1;
						b = m;
						break;

				case 1:
						r = mid2;
						g = v;
						b = m;
						break;

				case 2:
						r = m;
						g = v;
						b = mid1;
						break;

				case 3:
						r = m;
						g = mid2;
						b = v;
						break;

				case 4:
						r = mid1;
						g = m;
						b = v;
						break;

				case 5:
						r = v;
						g = m;
						b = mid2;
						break;
			}
	}
	return vec3(r,g,b);
}

vec4 hsv2rgb(float hue, float sat, float val, float alpha)
{
	vec3 hsv = hsv2rgb(hue, sat, val);
	return vec4(hsv.x, hsv.y, hsv.z, alpha);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Create a NULL-terminated string by reading the provided file
// Note: this seems to expect a unix text file
static char* readShaderSource(const char* shaderFile)
{
	FILE* fp = fopen(shaderFile, "r");

	if ( fp == NULL ) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// Create a GLSL program object from vertex and fragment shader files
GLuint loadAndInitializeShaders(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
			const char*  filename;
			GLenum       type;
			GLchar*      source;
		};  
		
	Shader shaders[2] = {
			{ vShaderFile, GL_VERTEX_SHADER, NULL },
			{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
		};

	GLuint program = glCreateProgram();


	for ( int i = 0; i < 2; ++i ) {
		Shader& s = shaders[i];

		std::cout << "loading shader '" << s.filename << "'" << std::endl;

		s.source = readShaderSource( s.filename );
		if ( shaders[i].source == NULL ) {
			std::cerr << "Failed to read " << s.filename << std::endl;
			exit( EXIT_FAILURE );
		}

		// output the shader
		std::cout << std::endl;
		std::cout << s.source << std::endl;
		std::cout << std::endl;

		GLuint shader = glCreateShader( s.type );
		glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );

		std::cout << "compiling shader" << std::endl;

		glCompileShader( shader );

		GLint  compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled ) {
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
			char* logMsg = new char[logSize];
			glGetShaderInfoLog( shader, logSize, NULL, logMsg );
			std::cerr << logMsg << std::endl;
			delete [] logMsg;

			exit( EXIT_FAILURE );
		}

		delete [] s.source;

		glAttachShader( program, shader );
	}

	/* link  and error check */
	std::cout << "linking shaders" << std::endl;
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
	if ( !linked ) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog( program, logSize, NULL, logMsg );
		std::cerr << logMsg << std::endl;
		delete [] logMsg;

		exit( EXIT_FAILURE );
	}

	return program;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// get attribute location with an error check
int glGetAttribLocationHelper(GLuint program, char* name, bool exitOnError)
{

	// get the position of the attribute 
	int loc = glGetAttribLocation( program, name );
	if (loc < 0)
	{
		std::cerr << "attribute '" << name << 
			"' not found in shader program " << program << std::endl;
		if (exitOnError)
			exit(0);
	}
	return loc;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// get uniform location with an error check
int glGetUniformLocationHelper(GLuint program, char* name, bool exitOnError)
{

	// get the position of the uniform 
	int loc = glGetUniformLocation( program, name );
	if (loc < 0)
	{
		std::cerr << "uniform '" << name << 
			"' not found in shader program " << program << std::endl;
		if (exitOnError)
			exit(0);
	}
	return loc;
}




}