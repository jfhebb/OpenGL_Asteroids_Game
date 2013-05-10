#version 120

uniform vec4 in_Colour;
attribute vec4 in_Position;
varying vec4 v_Colour;

void main()
{
		gl_Position = in_Position;
		v_Colour = in_Colour;
}

