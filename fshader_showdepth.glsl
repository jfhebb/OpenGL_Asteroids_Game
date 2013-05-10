#version 120

varying vec4 v_Colour;

void main()
{
    gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
