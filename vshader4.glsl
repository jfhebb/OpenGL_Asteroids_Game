#version 120

uniform float d;
attribute vec4 in_Position;

void main()
{
		vec4 p = in_Position;

		// x
    gl_Position[0] = (-d * in_Position[0]) / in_Position[2];
    // y
     gl_Position[1] = (-d * in_Position[1]) / in_Position[2];   
    // z
     gl_Position[2] = 0.0;  
     
     gl_Position[3]  = 1.0;
}

