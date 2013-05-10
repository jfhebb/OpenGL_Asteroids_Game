#version 120
/*
Hack Vertex Shader for lighting a sphere only
doesn't take into account normals at all
only send diffuse material colour
*/

// HACK: we'll use the materialColour for ambient, diffuse, and specular
// and hardcode specular reflection
uniform vec4 materialColour;
vec4 materialAmbient;
vec4 materialDiffuse;
vec4 materialSpecular;
float materialSpecularExp = 100;

// HACK: except for position, lighting properties are hard coded
uniform vec4 lightPosition;
vec4 lightAmbient = vec4(0.1, 0.1, 0.1, 1);
vec4 lightDiffuse = vec4(0.8, 0.8, 0.8, 1);
vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1);

// vertex information
// - - - - - - - - 

// transformation matrices
uniform mat4 modelView; 
uniform mat4 projection; 

// pass in a vertex position and vertex normal
attribute vec4 vPosition; 

// pass computed colour to the fragment shader
varying vec4 vColour;

 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void main()
{
		// transform the vertex position to clip coordinates
		gl_Position = projection * modelView * vPosition;

		
		// lighting
		// - - - - - - - - - -
		// we calculate lighting in view (or eye) coordinates
		
		// transform vertex, normal to view coordinate
		vec4 vertexPos = modelView * vPosition;
		
		// HACK: for a sphere centred at the origin, 
		// we can use the vertex position for
		// the surface normal at this vertex
		vec3 N = normalize(vPosition.xyz);
		
		// calculate the light direction
		// we multiply vertexPos by lightPos.w to accomodate 
		// directional lights (lightPos is a vector, w == 0) and 
		// point lights (lightPosition is a point, w == 1)
		
		vec3 L;
		
		// check for directional or point light
		// WARNING: if/then statements can be costly in shader code
		if (lightPosition.w == 0)
		{
			L = normalize(lightPosition).xyz;
		}
		else
		{
			L = vec3(normalize(lightPosition - vertexPos));
		}
	
		// eye vector (vertexPos - origin, which is always COP at 0,0,0)
		vec3 E = -normalize(vertexPos.xyz);
		
		// half-way vector for Blinn-Phong
		vec3 H = normalize(L + E);		
			
		// ambient contribution, La is hardcoded 
		materialAmbient = materialColour;
		vec4 ambient = materialAmbient * lightAmbient;
		
		// diffuse contribution, Ld is hardcoded 
		materialDiffuse = materialColour;
		vec4 diffuse = max(dot(L,N), 0.00001) * materialDiffuse * lightDiffuse;

		// specular contribution, Ls and exp are hard coded 
		materialSpecular = materialColour;
		
		vec4 specular = pow(max(dot(H,N),0.00001), materialSpecularExp) 
										* materialSpecular * lightSpecular;
		
		// final colour is the sum of all contributions
		vColour = ambient + diffuse + specular;
		
		//vColour = vec4(1,0,0,1);  // useful to debug shader		
}







