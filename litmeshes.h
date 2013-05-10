#ifndef DJV_LITMESHES_H_
#define DJV_LITMESHES_H_

#include <vector>

#include "gl_include.h"

#include "vec.h"


namespace djv {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// abstract Mesh class with normals
class LitMesh
{
public:
	// initialize all attribute locations from the shader program
	static void initAttributeLocations(GLuint program);

	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void visualizeNormals() { };

protected:

	// mesh buffer ids
	GLuint bufferId_vertices;
	GLuint bufferId_indices;

	// number of indices or vertices to draw
	int drawNum;

	//  bytes in each vertex
	size_t vertexStride; 

	// offset in each vertex for normal
	size_t normalOffset; 

	// a buffer to visualize surface normals as lines
	GLuint bufferId_visualizeNormals;
	int visualizeNormalsDrawNum;
	size_t visualizeNormalVertexStride;

	// the shader attribute location for vertex position
	static GLint attributeId_vPosition;
	// the shader attribute location for normal position
	static GLint attributeId_vNormal;

	// helpful data structure for creating meshes
	struct Vertex
	{
		Vertex(vec3 v, vec3 n) {
			vertex = v;
			normal = n;
		}

		vec3 vertex;
		vec3 normal;
	};

	// helper function to add a quadrilateral face with normal
void LitMesh::addQuadFace(vec3 a, vec3 b, vec3 c, vec3 d, vec3 n, 
					      std::vector< Vertex >* vertices, 
						  std::vector< GLushort >* indices,
						  std::vector< vec3 >* visualizeNormals = NULL
						  );

};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class LitCubeMesh : public LitMesh
{
public:
	void init();
	void draw();
	void visualizeNormals();
};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class LitSphereMesh : public LitMesh
{
public:
	void init() { init(4); }
	void init(int n);
	void draw();
	void visualizeNormals();

private:
	std::vector< vec4 > divide_triangle(vec4 a, vec4 b, vec4 c, int n);
	vec4 unit(const vec4 &p);
};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class LitCylinderMesh : public LitMesh
{
public:
	void init() { init(22,1); }
	void init(int facets, int segments);
	void draw();
	void visualizeNormals();
};

}

#endif