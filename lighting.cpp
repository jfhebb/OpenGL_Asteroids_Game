#include "lighting.h"

#include "gl_utilities.h"

namespace djv {

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// light properties

void Light::initUniformLocations(GLuint program)
{
	id_position  = glGetUniformLocationHelper(program, "light.position");
	id_ambient  = glGetUniformLocationHelper(program, "light.ambient");
	id_diffuse  = glGetUniformLocationHelper(program, "light.diffuse");
	id_specular  = glGetUniformLocationHelper(program, "light.specular");
}

void Light::sendUniforms(mat4 View)
{
	glUniform4fv(id_position, 1, View * position);
	glUniform4fv(id_ambient, 1, ambient);
	glUniform4fv(id_diffuse, 1, diffuse);
	glUniform4fv(id_specular, 1, specular);
}

// C++ likes to define static types again
GLint Light::id_position;
GLint Light::id_ambient;
GLint Light::id_diffuse;
GLint Light::id_specular;


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// shading characteristics of objects

void Material::initUniformLocations(GLuint program)
{
	id_emissive  = glGetUniformLocationHelper(program, "material.emissive");
	id_ambient  = glGetUniformLocationHelper(program, "material.ambient");
	id_diffuse  = glGetUniformLocationHelper(program, "material.diffuse");
	id_specular  = glGetUniformLocationHelper(program, "material.specular");
	id_specularExp  = glGetUniformLocationHelper(program, "material.specularExp");
}

void Material::sendUniforms()
{
	glUniform4fv(id_emissive, 1, emissive);
	glUniform4fv(id_ambient, 1, ambient);
	glUniform4fv(id_diffuse, 1, diffuse);
	glUniform4fv(id_specular, 1, specular);
	glUniform1f(id_specularExp, specularExp);

}

// C++ likes to define static types again
GLint Material::id_emissive;
GLint Material::id_ambient;
GLint Material::id_diffuse;
GLint Material::id_specular;
GLint Material::id_specularExp;


}