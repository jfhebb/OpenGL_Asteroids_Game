#include "litmeshes.h"

#include "gl_utilities.h"

namespace djv {

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

GLint LitMesh::attributeId_vPosition = -1;
GLint LitMesh::attributeId_vNormal = -1;


void LitMesh::initAttributeLocations(GLuint program)
{
	attributeId_vPosition  = glGetAttribLocationHelper(program, "vPosition");
	glEnableVertexAttribArray(attributeId_vPosition);
	attributeId_vNormal  = glGetAttribLocationHelper(program, "vNormal");
	glEnableVertexAttribArray(attributeId_vNormal);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


// a,b,c,d must be given in CCW order
void LitMesh::addQuadFace(vec3 a, vec3 b, vec3 c, vec3 d, vec3 n, 
					      std::vector< Vertex >* vertices, 
						  std::vector< GLushort >* indices,
						  std::vector< vec3 >* visualizeNormals  )
{

	// CCW order 
	// (not necessarily with a in bottom left, could be rotated)
	//  d-------c 
	//  |       | 
	//  |       |
	//  |       |
	//  a-------b

	int i = vertices->size();

	// add the vertices
	vertices->push_back(Vertex(a, n));
	vertices->push_back(Vertex(b, n));
	vertices->push_back(Vertex(c, n));
	vertices->push_back(Vertex(d, n));

	// add the indices for two triangles
	indices->push_back(i);
	indices->push_back(i+1);
	indices->push_back(i+2);

	indices->push_back(i);
	indices->push_back(i+2);
	indices->push_back(i+3);

	// optional, but useful for debugging normals
	// create some vertices to draw lines to visualize this
	// surface's normal
	if (visualizeNormals != NULL)
	{
		// find centre point
		vec3 p = a + 0.5 * (d- a) + 0.5 * (b - a);

		// add vertices for the normal visualization
		visualizeNormals->push_back(p);
		visualizeNormals->push_back(p + 0.25 * n);
	}
}


void LitCubeMesh::init()
{

	//    v7----- v6
	//   /|      /|
	//  v3------v2|
	//  | |     | |
	//  | |v4---|-|v5
	//  |/      |/
	//  v0------v1

	// the vertices that make up a cube
	vec3 v[] = {
			vec3( -0.5f, -0.5f,  0.5f ),  // v0
			vec3(  0.5f, -0.5f,  0.5f ),  // v1
			vec3(  0.5f,  0.5f,  0.5f ),  // v2
			vec3( -0.5f,  0.5f,  0.5f ),  // v3
			vec3( -0.5f, -0.5f, -0.5f ),  // v4
			vec3(  0.5f, -0.5f, -0.5f ),  // v5
			vec3(  0.5f,  0.5f, -0.5f ),  // v6
			vec3( -0.5f,  0.5f, -0.5f )   // v7
	};

	// to light the cube, we need to create faces with normals
	// these are the 'actual' vertices we send to the GPU
	std::vector< Vertex > vertices;
	std::vector< GLushort > indices;

	std::vector< vec3 > visualizeNormals;

	vec3 n;


	// front 
	n = vec3(0,0,1);
	addQuadFace(v[0],v[1],v[2],v[3], n, &vertices, &indices, &visualizeNormals);

	// right
	n = vec3(1,0,0);
	addQuadFace(v[1],v[5],v[6],v[2], n, &vertices, &indices, &visualizeNormals);

	// left
	n = vec3(-1,0,0);
	addQuadFace(v[4],v[0],v[3],v[7], n, &vertices, &indices, &visualizeNormals);

	// back
	n = vec3(0,0,-1);
	addQuadFace(v[5],v[4],v[7],v[6], n, &vertices, &indices, &visualizeNormals);

	// top
	n = vec3(0,1,0);
	addQuadFace(v[3],v[2],v[6],v[7], n, &vertices, &indices, &visualizeNormals);

	// bottom
	n = vec3(0,-1,0);
	addQuadFace(v[0],v[4],v[5],v[1], n, &vertices, &indices, &visualizeNormals);

	drawNum = indices.size();

	vertexStride = sizeof(vertices[0]);
	normalOffset = sizeof(vertices[0].vertex);

	visualizeNormalsDrawNum = visualizeNormals.size();


	// Create vertex buffer
	glGenBuffers(1, &bufferId_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), 
			&vertices.front(), GL_STATIC_DRAW);

	// Create index buffer
	glGenBuffers(1, &bufferId_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), 
			&indices.front(), GL_STATIC_DRAW);

	// Create normal visualization buffer
	glGenBuffers(1, &bufferId_visualizeNormals);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(visualizeNormals[0]) * visualizeNormals.size(), 
			&visualizeNormals.front(), GL_STATIC_DRAW);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitCubeMesh::draw()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId_indices);

	// map the buffer data to attribute locations
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 3, GL_FLOAT, GL_FALSE, 
		vertexStride, BUFFER_OFFSET(0));
	glVertexAttribPointer(LitMesh::attributeId_vNormal, 3, GL_FLOAT, GL_FALSE, 
		vertexStride, BUFFER_OFFSET(normalOffset));

	// draw the mesh
	glDrawElements(GL_TRIANGLES, drawNum, GL_UNSIGNED_SHORT, NULL);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitCubeMesh::visualizeNormals()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 3, GL_FLOAT, GL_FALSE, 
		0, BUFFER_OFFSET(0));
	glDrawArrays(GL_LINES, 0, visualizeNormalsDrawNum);

}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void LitSphereMesh::init(int n)
{
	std::vector< vec4 > vertices;

	// 4 points on a tetrahedron
	vec4 v[4]= {	vec4(0.0, 0.0, 1.0, 1.0), 
					vec4(0.0, 0.942809, -0.333333, 1.0),
					vec4(-0.816497, -0.471405, -0.333333, 1.0),
					vec4(0.816497, -0.471405, -0.333333, 1.0) 	};

	// recursive subdivision, add to vertex list
	std::vector< vec4 > t;
	t =	divide_triangle(v[0], v[1], v[2] , n);
	vertices.insert( vertices.end(), t.begin(), t.end() );
	t = divide_triangle(v[3], v[2], v[1], n );
	vertices.insert( vertices.end(), t.begin(), t.end() );
	t = divide_triangle(v[0], v[3], v[1], n );
	vertices.insert( vertices.end(), t.begin(), t.end() );
	t = divide_triangle(v[0], v[2], v[3], n );
	vertices.insert( vertices.end(), t.begin(), t.end() );

	// put sphere vertices in buffer
	glGenBuffers(1, &bufferId_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	vertexStride = sizeof(vertices[0]);
	drawNum = vertices.size();


	// create lines to visualize normals
	std::vector< vec3 > visualizeNormals;

	for (int i = 0; i < vertices.size(); i++)
	{
		vec4 v4 = vertices[i];
		vec3 v(v4.x, v4.y, v4.z);

		visualizeNormals.push_back(v);
		visualizeNormals.push_back(v + (0.3 * v));
	}

	visualizeNormalsDrawNum = visualizeNormals.size();

	// load into normal visualization buffer
	glGenBuffers(1, &bufferId_visualizeNormals);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(visualizeNormals[0]) * visualizeNormals.size(), 
			&visualizeNormals.front(), GL_STATIC_DRAW);

	std::cout << "Init sphere with " << n << " subdivisions resulting in " << vertices.size() << " vertices." << std::endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

std::vector< vec4 > LitSphereMesh::divide_triangle(vec4 a, vec4 b, vec4 c, int n)
{
	std::vector< vec4 > vertices;
	vec4 v1, v2, v3;
	// recurse
	if(n > 0)
	{
		v1 = unit(a + b);
		v2 = unit(a + c);
		v3 = unit(b + c);   

		std::vector< vec4 > t;
		t =	divide_triangle(a ,v2, v1, n-1);
		vertices.insert( vertices.end(), t.begin(), t.end() );
		t = divide_triangle(c ,v3, v2, n-1);
		vertices.insert( vertices.end(), t.begin(), t.end() );
		t = divide_triangle(b ,v1, v3, n-1);
		vertices.insert( vertices.end(), t.begin(), t.end() );
		t = divide_triangle(v1 ,v2, v3, n-1);
		vertices.insert( vertices.end(), t.begin(), t.end() );
		return vertices;
	}
	// end recursion
	else 
	{
		// just create a triangle and return it
		std::vector< vec4 > t;
		t.push_back(a);
		t.push_back(b);
		t.push_back(c);
		//t =	triangle(a, b, c);
		vertices.insert( vertices.end(), t.begin(), t.end() );
		return vertices;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// helper function to 'push' vertex to circle 
vec4 LitSphereMesh::unit(const vec4 &p)
{
	vec4 c;
	double d = 0.0f;
	for(int i=0; i < 3; i++) 
	{
		d += p[i] * p[i];
	}
	d = sqrt(d);
	if(d > 0.0) 
	{
		for(int i=0; i<3; i++)
		{
			c[i] = p[i] / d;
		}
	}
	c[3] = 1.0f;
	return c;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitSphereMesh::draw()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);

	// map the buffer data to attribute locations
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 4, GL_FLOAT, GL_FALSE, 
		0, BUFFER_OFFSET(0));
	glVertexAttribPointer(LitMesh::attributeId_vNormal, 4, GL_FLOAT, GL_FALSE, 
		0, BUFFER_OFFSET(0));

	// draw the mesh
	glDrawArrays(GL_TRIANGLES, 0, drawNum);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitSphereMesh::visualizeNormals()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 3, GL_FLOAT, GL_FALSE, 
		0, BUFFER_OFFSET(0));
	glDrawArrays(GL_LINES, 0, visualizeNormalsDrawNum);

}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void LitCylinderMesh::init(int facets, int segments)
{
	std::vector< Vertex > vertices;

	vec3 n; // vertex normal

	// top face vertices first
	n = vec3(0,1,0);

	// centre
	vertices.push_back(Vertex(vec3(0,0.5,0),n));
	 
	//ring
	for(int f = 0; f < facets; f++)
	{
		float angle_rad = (float)f / (float)facets * (float)(2.0f * M_PI);
		vec3 v(sin(angle_rad) * 0.5f, 0.5, cos(angle_rad) * 0.5f);
		vertices.push_back(Vertex(v, n));
	}

	// bottom face vertices next
	n = vec3(0,-1,0);

	// centre
	vertices.push_back(Vertex(vec3(0,-0.5,0), vec3(0,-1,0)));

	//ring
	for(int f = 0; f < facets; f++)
	{
		float angle_rad = (float)f / (float)facets * (float)(2.0f * M_PI);
		vec3 v(sin(angle_rad) * 0.5f, -0.5, cos(angle_rad) * 0.5f);
		vertices.push_back(Vertex(v, n));
	}

	// side vertices

	// compute segment spacing
	float seg_space = 1.0f / (float)segments;

	// start at the top layer
	float top = 0.5; 

	// create rings of vertices for each segment
	for(int s = 0; s <= segments; s++)
	{
		float layer = top - (s * seg_space);

		for(int f = 0; f < facets; f++)
		{
			float angle_rad = (float)f / (float)facets * (float)(2.0f * M_PI);
			vec3 v(sin(angle_rad) * 0.5f, layer, cos(angle_rad) * 0.5f);
			n = vec3(sin(angle_rad), 0, cos(angle_rad)); 
			vertices.push_back(Vertex(v, n));
		}
	}

	// create the mesh triangles
	// could use a fan for the top and bottom and strips for the sides
	// but then you need multiple glDraw calls
	// often easiest to just make triangles

	// create indices for filled cylinder
	std::vector< GLushort > indices;

	// top cap
	int center = 0;
	int offset = center + 1;
	for(int f = 0; f < facets; f++)
	{
		// CCW triangle from top
		indices.push_back(center);
		int i = f + offset;
		indices.push_back(i);
		indices.push_back(f < facets - 1 ? i + 1 : offset);

	}

	// bottom cap
	center = 1 + facets;
	offset = center + 1;
	for(int f = 0; f < facets; f++)
	{
		// CCW triangle from bottom
		indices.push_back(center);
		int i = f + offset;
		indices.push_back(f < facets - 1 ? i + 1 : offset);
		indices.push_back(i);
	}

	

	// sides
	for(int s = 0; s < segments; s++)
	{
		offset = 2 * (facets + 1)  + (facets * s);
		for(int f = 0; f < facets; f++)
		{
			int i = f + offset;

			int iPlus1 = f < facets - 1 ? i + 1 : offset;

			// triangle 1
			indices.push_back(i);
			indices.push_back(i + facets);
			indices.push_back(iPlus1);


			// triangle 2
			indices.push_back(iPlus1);
			indices.push_back(i + facets);
			indices.push_back(iPlus1 + facets);

		}
	}

	for (unsigned int i=0; i < indices.size(); i++)
	{
		if (indices[i] >  vertices.size() - 1 )
		{
			std::cout << "index error" << std::endl;
		}
	}

	drawNum = indices.size();
	vertexStride = sizeof(vertices[0]);
	normalOffset = sizeof(vertices[0].vertex);

	// load all the buffers
	
	// load into vertex buffer
	glGenBuffers(1, &bufferId_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// load into the vertex buffer
	drawNum = indices.size();
	glGenBuffers(1, &bufferId_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	// create lines to visualize normals
	std::vector< vec3 > visualizeNormals;

	for (int i = 0; i < vertices.size(); i++)
	{
		vec3 v = vertices[i].vertex;
		vec3 n = vertices[i].normal;
		visualizeNormals.push_back(v);
		visualizeNormals.push_back(v + (0.3 * n));
	}

	visualizeNormalsDrawNum = visualizeNormals.size();

	// load into normal visualization buffer
	glGenBuffers(1, &bufferId_visualizeNormals);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(visualizeNormals[0]) * visualizeNormals.size(), 
			&visualizeNormals.front(), GL_STATIC_DRAW);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitCylinderMesh::draw()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId_indices);

	// map the buffer data to attribute locations
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 3, GL_FLOAT, GL_FALSE, 
		vertexStride, BUFFER_OFFSET(0));
	glVertexAttribPointer(LitMesh::attributeId_vNormal, 3, GL_FLOAT, GL_FALSE, 
		vertexStride, BUFFER_OFFSET(normalOffset));

	// draw the mesh
	glDrawElements(GL_TRIANGLES, drawNum, GL_UNSIGNED_SHORT, NULL);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void LitCylinderMesh::visualizeNormals()
{
	// bind the vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, bufferId_visualizeNormals);
	glVertexAttribPointer(LitMesh::attributeId_vPosition, 3, GL_FLOAT, GL_FALSE, 
		0, BUFFER_OFFSET(0));
	glDrawArrays(GL_LINES, 0, visualizeNormalsDrawNum);

}



















}