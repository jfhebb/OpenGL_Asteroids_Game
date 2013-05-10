#version 120

// shader for perspective demo
uniform vec3 pos;
uniform float d;
attribute vec4 in_Position;

void main()
{
		vec4 p = in_Position;

    gl_Position[0] = 	(-d * (in_Position[0] + pos[0])) / 
    									(in_Position[2] + pos[2]);

     gl_Position[1] = (-d * (in_Position[1] + pos[1])) / 
     									(in_Position[2] + pos[2]);   
    // set z and w
    gl_Position[2] = 0.0;  // what happens if |z| > 1?
    gl_Position[3]  = 1.0; // homogenous coordinates
}

