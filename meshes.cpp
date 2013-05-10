#include "meshes.h"

#include "gl_utilities.h"

namespace djv {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// utility function to display a wire mesh
void displayWireMesh(Mesh* mesh, vec4 colour, int uniformId_colour)
{
	glUniform4fv(uniformId_colour, 1, colour);
	glEnable(GL_POLYGON_OFFSET_FILL);
	// trick to make sure the filled part is 'deeper' than the wire
	// otherwise the wire and fill fragments may flip
	glPolygonOffset(1,1); 
	mesh->draw(true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	// white 'wireframe' colour
	glUniform4fv(uniformId_colour, 1, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	mesh->draw(false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

GLint Mesh::in_position_loc = -1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Mesh::Mesh(void)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Mesh::~Mesh(void)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Mesh::init()
{
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


GridMesh::GridMesh(int numX, int numY, float sizeSquare)
{
	this->numX = numX;
	this->numY = numY;
	this->sizeSquare = sizeSquare;
}

void GridMesh::init()
{
	std::vector< vec2 > vertices;

	float width = numX * sizeSquare;
	float height = numY * sizeSquare;

	// draw vertical lines
	for (int i = -numX; i <= numX; i++)
	{
		float x = i * sizeSquare;
		vertices.push_back(vec2(x, -height));
		vertices.push_back(vec2(x, height));
	}

	// draw horizontal lines
	for (int i = -numY; i <= numY; i++)
	{
		float y = i  * sizeSquare;
		vertices.push_back(vec2(-width, y));
		vertices.push_back(vec2(width, y));
	}

	drawNum = vertices.size();
	stride = sizeof(vertices[0]);

	// Create vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, stride * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	// describe how the shader attributes appear in the array buffer
	//glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));


}

void GridMesh::draw(bool filled)
{
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glVertexAttribPointer(Mesh::in_position_loc, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glDrawArrays(GL_LINES, 0, drawNum);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void CubeMesh::init()
{


	//    v7----- v6
	//   /|      /|
	//  v3------v2|
	//  | |     | |
	//  | |v4---|-|v5
	//  |/      |/
	//  v0------v1

	GLfloat vertices[][3] = {
			{ -0.5f, -0.5f,  0.5f },  // v0
			{  0.5f, -0.5f,  0.5f },  // v1
			{  0.5f,  0.5f,  0.5f },  // v2
			{ -0.5f,  0.5f,  0.5f },  // v3
			{ -0.5f, -0.5f, -0.5f },  // v4
			{  0.5f, -0.5f, -0.5f },  // v5
			{  0.5f,  0.5f, -0.5f },  // v6
			{ -0.5f,  0.5f, -0.5f }   // v7
	};


	GLushort indices[] = {

			// solid cube

			// front
			0,1,2,
			0,2,3,

			// right
			1,5,6,
			1,6,2,

			// left
			4,0,3,
			4,3,7,

			// back
			5,4,7,
			5,7,6,

			// top
			3,2,6,
			3,6,7,

			// bottom
			1,0,4,
			1,4,5
	};

	GLubyte wireIndices[] = {

			// wire cube

			// front
			0,1,1,2,2,3,3,0,

			// back
			5,4,4,7,7,6,6,5,

			// sides
			0,4,
			3,7,
			1,5,
			2,6
	};

	drawNum = 36;
	stride = sizeof(vec3);

	wireIndexNum = 24;

	// Create vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create index buffer
	glGenBuffers(1, &index_bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Create index buffer
	glGenBuffers(1, &wireIndex_bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIndex_bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wireIndices), wireIndices, GL_STATIC_DRAW);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CubeMesh::draw(bool filled)
{
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);

	if (filled)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bufferId);
		glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
		glDrawElements(GL_TRIANGLES, drawNum, GL_UNSIGNED_SHORT, NULL);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIndex_bufferId);
		glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
		glDrawElements(GL_LINES, wireIndexNum, GL_UNSIGNED_BYTE, NULL);
	}
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void CylinderMesh::init()
{
	init(22,1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CylinderMesh::init(int facets, int segments)
{
	std::vector< vec3 > vertices;

	// compute segment spacing
	float seg_space = 1.0f / (float)segments;

	// start at the top layer
	float top = 0.5; 

	// top cap vertex
	vertices.push_back(vec3(0,0.5,0));

	// create rings of vertices for each segment
	for(int s = 0; s <= segments; s++)
	{
		float layer = top - (s * seg_space);

		for(int f = 0; f < facets; f++)
		{
			float angle_rad = (float)f / (float)facets * (float)(2.0f * M_PI);
			vec3 v(sin(angle_rad) * 0.5f, layer, cos(angle_rad) * 0.5f);
			vertices.push_back(v);
		}
	}

	// bottom cap vertex
	vertices.push_back(vec3(0,-0.5,0));

	//cylinder.numVertices = vertices.size();

	// create the vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	stride = sizeof(vertices[0]);

	// create indices for filled cylinder
	std::vector< GLushort > indices;

	// top cap
	int offset = 1;
	for(int f = 0; f < facets; f++)
	{
		indices.push_back(0);
		int i = f + offset;
		indices.push_back(f < facets - 1 ? i + 1 : offset);
		indices.push_back(i);
	}

	// bottom cap
	offset = 1 + (facets * segments);
	int bottom_middle = 1 + (facets * (segments + 1));
	for(int f = 0; f < facets; f++)
	{
		indices.push_back(bottom_middle);
		int i = f + offset;
		indices.push_back(f < facets - 1 ? i + 1 : offset);
		indices.push_back(i);
	}

	// sides
	for(int s = 0; s < segments; s++)
	{
		offset = 1 + (facets * s);
		for(int f = 0; f < facets; f++)
		{
			int i = f + offset;

			int iPlus1 = f < facets - 1 ? i + 1 : offset;

			// triangle 1
			indices.push_back(i);
			indices.push_back(iPlus1);
			indices.push_back(i + facets);

			// triangle 2
			indices.push_back(iPlus1);
			indices.push_back(iPlus1 + facets);
			indices.push_back(i + facets);
		}
	}

	for (unsigned int i=0; i < indices.size(); i++)
	{
		if (indices[i] >  vertices.size() - 1 )
		{
			std::cout << "index error" << std::endl;
		}
	}

	// load into buffer
	drawNum = indices.size();
	glGenBuffers(1, &index_bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	// create indices for wire cylinder
	std::vector< GLushort > wireIndices;

	// create rings of vertices for each segment
	for(int s = 0; s <= segments; s++)
	{
		for(int f = 0; f < facets; f++)
		{
			wireIndices.push_back(1 + (s * facets) + f); 
		}

		wireIndices.push_back(1 + (s * facets));
	}

	// load into buffer
	wireIndexNum = wireIndices.size();
	glGenBuffers(1, &wireIndex_bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIndex_bufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wireIndices[0]) * wireIndices.size(), &wireIndices.front(), GL_STATIC_DRAW);

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CylinderMesh::draw(bool filled)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);

	if (filled)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bufferId);
		glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
		glDrawElements(GL_TRIANGLES, drawNum, GL_UNSIGNED_SHORT, NULL);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIndex_bufferId);
		glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
		glDrawElements(GL_LINE_STRIP, wireIndexNum, GL_UNSIGNED_SHORT, NULL);
	}
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void SphereMesh::init(int n)
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
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
	stride = sizeof(vertices[0]);
	drawNum = vertices.size();

	std::cout << "Init sphere with " << n << " subdivisions resulting in " << vertices.size() << " vertices." << std::endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

std::vector< vec4 > SphereMesh::divide_triangle(vec4 a, vec4 b, vec4 c, int n)
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
		t =	divide_triangle(a ,v2, v1, n-1) ;
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

		vertices.insert( vertices.end(), t.begin() , t.end());
		return vertices;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// helper function to 'push' vertex to circle 
vec4 SphereMesh::unit(const vec4 &p)
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

void SphereMesh::draw(bool filled)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glVertexAttribPointer(Mesh::in_position_loc, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glDrawArrays(GL_TRIANGLES, 0, drawNum);
}

void Stars::init()
{
	std::vector< vec3 > vertices;

	for(int i =0; i < 4000; i++)
	{	
		vertices.push_back(vec3(randRange(-130,130),randRange(-130,130),randRange(-130,130)));

	}
	
	

	drawNum = vertices.size();
	stride = sizeof(vertices[0]);

	// Create vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, stride * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



void Stars::draw(bool filled)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glPointSize(1.2f);
	glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glDrawArrays(GL_POINTS, 0, drawNum);
	
}


// START PARTICLES
void shipParticles::init()
{
	std::vector< vec3 > vertices;

	for(int f = 0; f < 12; f++)
	{
			float angle_rad = (float)f/30 * (float)(2.0f * M_PI);
			float x_bound = sin(angle_rad) * 0.15f;
			float z_bound = cos(angle_rad) * 0.15f;

			vec3 v(randRange(-x_bound,x_bound), 0, randRange(-z_bound,z_bound));
			vertices.push_back(v);

	}

	drawNum = vertices.size();
	stride = sizeof(vertices[0]);

	// Create vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, stride * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



void shipParticles::draw(bool filled)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glPointSize(2.0f);
	glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	glDrawArrays(GL_POINTS, 0, drawNum);
	
}
//end particles

void Ship::init()
{
	std::vector< vec3 > vertices;

	int radius = 60;

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(0,0,-1));

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(-1,0.6,0));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(-1,0.6,0));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(0,1,0));

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(0,1,0));

	vertices.push_back(vec3(0,1,0));
	vertices.push_back(vec3(-1,0.6,0));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(2,0,0));

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(2,0,0));

	vertices.push_back(vec3(2,0,0));
	vertices.push_back(vec3(0,1,0));

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(0.3,-0.1,3));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(0.3,-0.1,-3));

	//

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(-0.6,0,1.3));

	vertices.push_back(vec3(-0.6,0,1.3));
	vertices.push_back(vec3(0.3,-0.1,3));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(-0.6,0,-1.3));

	vertices.push_back(vec3(-0.6,0,-1.3));
	vertices.push_back(vec3(0.3,-0.1,-3));

	//

	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(-0.3,1,1.3));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(-0.3,1,-1.3));

	//
	vertices.push_back(vec3(0,0,1));
	vertices.push_back(vec3(-1.6,1.5,1.5));

	vertices.push_back(vec3(0,0,-1));
	vertices.push_back(vec3(-1.6,1.5,-1.5));

	//
	vertices.push_back(vec3(-1.6,1.5,1.5));
	vertices.push_back(vec3(-0.3,1,1.3));

	vertices.push_back(vec3(-1.6,1.5,-1.5));
	vertices.push_back(vec3(-0.3,1,-1.3));

	drawNum = vertices.size();
	stride = sizeof(vertices[0]);

	// Create vertex buffer
	glGenBuffers(1, &vertex_bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glBufferData(GL_ARRAY_BUFFER, stride * vertices.size(), &vertices.front(), GL_STATIC_DRAW);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



void Ship::draw(bool filled)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bufferId);
	glLineWidth(1.5);
	glVertexAttribPointer(Mesh::in_position_loc, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));

	glDrawArrays(GL_LINES, 0, drawNum);
	
}



}