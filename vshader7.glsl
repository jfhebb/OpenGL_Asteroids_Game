#version 120

uniform vec4 in_Colour; 
uniform mat4 modelView; // transformation matrix
attribute vec4 in_Position; // vertex position
varying vec4 v_Colour;

void main()
{
		gl_Position = modelView * in_Position;
		v_Colour = in_Colour;
}

