#ifndef DJV_LIGHTING_H_
#define DJV_LIGHTING_H_


#include "vec.h"
#include "mat.h"


namespace djv {

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// light properties
class Light
{
public:

	// initialize all lighting uniform locations from the shader program
	static void initUniformLocations(GLuint program);

	// send all lighting information to the shader as a uniform
	void sendUniforms(mat4 View);

	// position or direction in world coordinates
	vec4 getPosition() { return position; }
	void setPosition(const vec4& pos) { position = pos; }

	// ambient components
	vec4 getAmbient() { return ambient; }
	void setAmbient(const vec4& components) { ambient = components; }

	// diffuse components
	vec4 getDiffuse() { return diffuse; }
	void setDiffuse(const vec4& components) { diffuse = components; }

	// specular components
	vec4 getSpecular() { return specular; }
	void setSpecular(const vec4& components) { specular = components; }

private:

	// lighting parameters
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// shader uniform ids
	static GLint id_position;
	static GLint id_ambient;
	static GLint id_diffuse;
	static GLint id_specular;
};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// shading characteristics of objects
class Material 
{
public:

	// initialize all material uniform locations from the shader program
	static void initUniformLocations(GLuint program);

	// send all material information to the shader as a uniform
	void sendUniforms();

	// emissive components
	vec4 getEmissive() { return emissive; }
	void setEmissive(const vec4& components) { emissive = components; }

	// ambient components
	vec4 getAmbient() { return ambient; }
	void setAmbient(const vec4& components) { ambient = components; }

	// diffuse components
	vec4 getDiffuse() { return diffuse; }
	void setDiffuse(const vec4& components) { diffuse = components; }

	// specular components
	vec4 getSpecular() { return specular; }
	void setSpecular(const vec4& components) { specular = components; }

	// specular components
	float getSpecularExp() { return specularExp; }
	void setSpecularExp(float exp) { specularExp = exp; }

private:

	// material parameters
	vec4 emissive;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float specularExp;


	// all the uniform ids in the shader
	static GLint id_emissive;
	static GLint id_ambient;
	static GLint id_diffuse;
	static GLint id_specular;
	static GLint id_specularExp;

};


}

#endif