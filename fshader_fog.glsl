#version 120

uniform vec4 fogColour; 
uniform float fogDensity; 
varying vec4 v_Colour;

void main()
{
		const float e = 2.71828;
		float fogfactor = (fogDensity * gl_FragCoord.z);
		fogfactor *= fogfactor;
		fogfactor = clamp(pow(e, -fogfactor), 0.0, 1.0);

    gl_FragColor = mix(fogColour, v_Colour, fogfactor);
}
