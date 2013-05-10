#version 120

uniform float offset;
attribute vec4 in_Position;
attribute vec4 in_Colour;
varying vec4 v_Colour;

void main()
{
		
		vec4 p = in_Position;
		
		p[0] += sin(offset) * 0.05;
		p[1] += cos(offset) * 0.05;
    gl_Position = p;
    v_Colour = in_Colour;
}

