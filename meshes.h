#ifndef DJV_MESHES_H_
#define DJV_MESHES_H_

#include <vector>

#include "gl_include.h"

#include "vec.h"


namespace djv {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// abstract Mesh class
class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	virtual void init() = 0;
	virtual void draw(bool filled) = 0;

	// the shader attribute location for vertex position
	static GLint in_position_loc;

protected:
	GLuint generateBufferId();
	GLuint vertex_bufferId;
	GLuint index_bufferId;

	int drawNum;
	int stride;

};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class GridMesh : public Mesh
{
public:
	GridMesh(int numX, int numY, float sizeSquare);
	void init();
	void draw(bool filled = true);
protected:
	int numX;
	int numY;
	float sizeSquare;

};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CubeMesh : public Mesh
{
public:
	void init();
	void draw(bool filled = true);

protected:
	GLuint wireIndex_bufferId;
	int wireIndexNum;

};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// radius 1.0 sphere mesh using subdivision
class SphereMesh : public Mesh
{
public:
	void init() { init(4); }
	void init(int n);
	void draw(bool filled = true);

private:
	std::vector< vec4 > divide_triangle(vec4 a, vec4 b, vec4 c, int n);
	vec4 unit(const vec4 &p);

};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CylinderMesh : public Mesh
{
public:
	void init();
	void init(int facets, int segments);
	void draw(bool filled = true);

protected:
	GLuint wireIndex_bufferId;
	int wireIndexNum;

};

class Stars : public Mesh
{
public:
	void init();
	void draw(bool filled = true);
protected:
	GLuint wireIndex_bufferId;
	int wireIndexNum;

};

class shipParticles : public Mesh
{
public:
	void init();
	void draw(bool filled = true);
protected:
	GLuint wireIndex_bufferId;
	int wireIndexNum;

};


class Ship : public Mesh
{
public:
	void init();
	void draw(bool filled = true);
protected:
	GLuint wireIndex_bufferId;
	int wireIndexNum;

};



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class HouseMesh : public Mesh
{
public:
	void init();
	void draw(bool filled = true);
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void displayWireMesh(Mesh* mesh, vec4 colour, int uniformId_colour);



}

#endif
