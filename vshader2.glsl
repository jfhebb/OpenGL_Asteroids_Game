#version 120

attribute vec4 in_Position;
attribute vec4 in_Colour;
varying vec4 v_Colour;

void main()
{
    gl_Position = in_Position;
    v_Colour = in_Colour;
}

