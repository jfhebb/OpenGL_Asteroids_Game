#version 120
/*
Vertex Shader with lighting
*/

// lighting and materials
// - - - - - - - - 

// material properties
struct Material 
{
	vec4 emissive;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float specularExp;
};

// lighting properties
struct Light
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform Material material;
uniform Light light;

// vertex information
// - - - - - - - - 

// transformation matrices
uniform mat4 modelView; 
uniform mat4 projection; 
uniform mat3 normalMatrix;

// pass in a vertex position and vertex normal
attribute vec4 vPosition; 
attribute vec3 vNormal;

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
		
		// the surface normal at this vertex
		// the 'normalMatrix' is the transpose of the 
		// inverse of modelView matrix
		vec3 N = normalize(normalMatrix * vNormal);
		
		// light should already be in view coordinates
		vec4 lightPos = light.position;
		
		// calculate the light direction
		// we multiply vertexPos by lightPos.w to accomodate 
		// directional lights (lightPos is a vector, w == 0) and 
		// point lights (lightPos is a point, w == 1)
		
		/*
		// this is slow because of the if/then statement, see below ...
		vec3 L;
		if (lightPos.w == 0)
		{
			L = normalize(lightPos).xyz;
		}
		else
		{
			L = vec3(normalize(lightPos - vertexPos));
		}
		*/
		// more efficient code to do same thing (if/else is costly in glsl)
		vec3 L = normalize(vec3(lightPos - vertexPos * lightPos.w));
		
		// eye vector (vertexPos - origin, which is always COP at 0,0,0)
		vec3 E = -normalize(vertexPos.xyz);
		
		// half-way vector for Blinn-Phong
		vec3 H = normalize(L + E);			
			
		// ambient contribution
		vec4 ambient = material.ambient * light.ambient;
		
		// diffuse contribution
		// note the value just above 0 due to curious shader FP errors 
		vec4 diffuse = max(dot(L,N), 0.00001) * material.diffuse * light.diffuse;

		// specular contribution
		vec4 specular = pow(max(dot(H,N),0.00001), material.specularExp) * material.specular * light.specular;
		
		// final colour is the sum of all contributions
		vColour = ambient + diffuse + specular +  material.emissive;
		
		//vColour = vec4(1,0,0,1);  // useful to debug shader		
}







