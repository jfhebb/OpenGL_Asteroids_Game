#version 120

uniform vec4 backColour; 
varying vec4 v_Colour;

void main()
{
	if (gl_FrontFacing)
    gl_FragColor = v_Colour;
  else
   	gl_FragColor = backColour;
}
