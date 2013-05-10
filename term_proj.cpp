// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>

// stl variable array is called a vector
#include <vector>
#include <stack>

// includes OpenGl libraries in a cross-platform way
#include "gl_include.h"

// include the helper files
#include "gl_utilities.h"

// include useful types for vectors and matrices
#include "vec.h"
#include "mat.h"
#include <math.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "Player.h"

// set-up some adjustable variables for
// interactive demonstrations
#include "Adjustable.h"

djv::Adjustable adjustable;


GLfloat X = 0; // adjustable lookAt position on 2D grid
GLfloat Y = 0;

GLfloat Z = 15; // adjustable eye distance

GLfloat R = 0; // rotation

// adjustable clipping parameters
GLfloat N = 1; // near plane
GLfloat F = 250; // far plane

GLfloat W = 0;
GLfloat A = 0;
GLfloat S = 0;
GLfloat D = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// use the djv namespace (avoid djv:: prefixes)
using namespace djv;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

GLuint program; // the OpenGL program "name"

// uniform variable 'name' ids 
GLint uniformId_colour;
GLint uniformId_modelView;

// geometry
#include "meshes.h"

CubeMesh cube; 
GridMesh grid(100, 100, 1.0f);
CylinderMesh cylinder;
Stars star;

SphereMesh sphere;	
Ship ship;
shipParticles particles;
Player playerA;


class Camera {

public:

	// constructor
	// fovy, aspect ration, eye direction vector
	Camera(vec4 dir, float f = 40.0f, float ar = 1.0f )
	{
		// want 1 unit direction vector
		// this would divide by zero if length of dir = 0
		eyeDirection = normalize(dir);

		fovy = f; 
		aspectRatio =  ar; 
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	mat4 getProjection()
	{
		// just assume a aspect ration of 1 for now ...
		return Perspective(fovy,aspectRatio,zNear,zFar);
	}
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// returns the current camera view
	// some views are based on the robot position
	mat4 getView()
	{
		vec4 eyePosition;
		vec4 up;

		// set the camera position directly
		if (!isTopView)
		{
			// keep  eye position relative to the lookAt position
			eyePosition = lookAtPosition + (dolly * eyeDirection);
			up = vec4(0,1,0,0);

		}
		else 
		{
			// eye position is directly up y-axis
			eyePosition = lookAtPosition + (dolly * vec4(0,1,0,0));
			// need to avoid a Y up direction, so pick up as the inverse
			// of the eyeDirection, that way it looks like the camera is
			// raise up and down, in the same orientation
			// (like peering over scene from front)
			up = -1 * eyeDirection;
		}

		// return 'camera' view transformation matrix
		return LookAt(eyePosition, lookAtPosition, up);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void setNear(float n) { zNear = n; }
	void setFar(float f) { zFar = f; }
	// consider x-z plane like x-y from outside interface
	void setLookAtPosition(float x, float y) { lookAtPosition = vec4(x, 0, y, 1.0); }
	void setEyeDirection(vec4 d) { eyeDirection = d; }
	vec4 getEyeDirection(void) { return eyeDirection; }

	void setDolly(float d) 
	{ 
		dolly = d; 
	}
	bool toggleTopView() 
	{ 
		return isTopView = !isTopView; 
	}

private:

	// view type
	bool isTopView;

	// camera position
	vec4 lookAtPosition;
	vec4 eyeDirection;
	float dolly;

	// camera parameters
	float fovy;
	float aspectRatio;
	float zNear;  
	float zFar;
	// NOTE: using 'near' or 'far' variable names will 
	// cause compile problems in Windows

};

// create a camera and specify an eye direction vector
Camera myCamera(vec4(-0.3,0.1f,-0.3,0));

// a bit of a hack to update all camera parameters using the global adjustable variables
void updateCamera()
{
	myCamera.setFar(F);
	myCamera.setNear(N);
	myCamera.setDolly(Z);
	myCamera.setLookAtPosition(X, Y);
}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// initialize the vertex and fragment shaders
void initShaders(void)
{
	std::cout << "initializing shaders" << std::endl;

	#if DEMO == 2
	program = loadAndInitializeShaders( "vshader7.glsl", "fshader_showdepth.glsl" );
	#else
	program = loadAndInitializeShaders( "vshader7.glsl", "fshader2.glsl" );
	#endif

	// get location id for uniform variables in shader program
	uniformId_colour = glGetUniformLocation(program, "in_Colour");
	uniformId_modelView = glGetUniformLocation(program, "modelView");
	
	//CHECK_GL_ERROR;

	glUseProgram( program );
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


void initGeometry( void )
{
	std::cout << "initializing geometry" << std::endl;

	// Create a vertex array object
	GLuint vao;
	#ifdef __APPLE__
		glGenVertexArraysAPPLE( 1, &vao );
		glBindVertexArrayAPPLE( vao );
	#else
		// if it crashes here, it could be because VertexArrays aren't supported
		// on your card, but perhaps an extension is available
		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );
	#endif

	// get the position of the attribute 
	// (should make a utility function here)
	int in_position_loc = glGetAttribLocation( program, "in_Position" );
	if (in_position_loc < 0)
	{
		std::cerr << "attribute 'in_Position' not found in shader program" << std::endl;
		exit(0);
	}
	
	// tell openGL we're using a custom attribute
	glEnableVertexAttribArray(in_position_loc);

	// set in_position_loc for all meshes using static variable
	Mesh::in_position_loc = in_position_loc;
	
	grid.init();
	cube.init();
	sphere.init(4);
	cylinder.init();
	star.init();
	ship.init();
	

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void displayWireCube(vec4 colour)
{
	glUniform4fv(uniformId_colour, 1, colour);
	glEnable(GL_POLYGON_OFFSET_FILL);
	// trick to make sure the filled part is 'deeper' than the wire
	// otherwise the wire and fill fragments may flip
	glPolygonOffset(1,1); 
	cube.draw(true);
	glDisable(GL_POLYGON_OFFSET_FILL);
	// white 'wireframe' colour
	glUniform4fv(uniformId_colour, 1, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	//cube.draw(false);
}

void displayWireCylinder(vec4 colour)
{
	glUniform4fv(uniformId_colour, 1, colour);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(5,5); 
	// Draw the cylinder (filling)
	cylinder.draw(true);
	
}

void displayWireSphere(vec4 colour)
{
	glUniform4fv(uniformId_colour, 1, colour);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(5,5); 
	sphere.draw(true);
}

void displayStars(vec4 colour)
{
	glUniform4fv(uniformId_colour, 1, colour);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(5,5); 
	star.draw(true);
	
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// The following are all the fields managed in the game

// Ship Fields
float speed = 0.0; // Current speed of the ship
float max_speed = speed;   // Maximum speed of the ship
vec4 current_dir(1,0,1,0); // Current direction of the ship
vec4 current_pos(0,0,0,0); // Current position of the ship
vec4 initial_dir(1,0,1,0); // Initial direction of the ship
float angle_rot = 0; // Angle to rotate the ship
float turn_rot = 0; // Rotate the direction vector for potential acceleration (up-key)

// Bullet fields
bool bullet_fired = false; // Boolean to check whether or not a bullet has been fired (space-key)
vec4 dir_vec(1,0,1,0); // Initial direction vevtor
std::vector< vec4 > bullet_positions; // Vector list of the positions of all bullets
std::vector< vec4 > bullet_directions; // Vector list of all the directions of the bullets

// Asteroids Fields
std::vector< vec4 > spheres; // Asteroids positions
std::vector< vec4 > sphere_dirs; // Asteroids directions
std::vector< float > sphere_angs; // Angular rotation of asteroid direction
std::vector< float > sphere_speed; // Asteroid speeds
std::vector< float > sphere_size; // Asteroid sizes (larger if shot)
int num_spheres = 60; // Number of spheres in the game at once
float radius = 20; // Radius of the explosion of an asteroid (from a missile)
float asteroid_speed = 0.01; // Asteroid speed
float ast_max_speed = 0.1; // Maximum speed of an asteroid
int killed_since_death = 0; // Number of orbs destroyed since either beginning or spawning (cannot die before killing one asteroid)

// Vector lists for particle fields
std::vector< mat4 > particle_dens;	// Matrix form translation of the particle fields
std::vector< shipParticles > particle_fields; // The actual particle fields (object of class shipParticles )

// missile fields
vec4 missileA_dir; // Direction of missile A
vec4 missileA_pos; // Position of missile A
vec4 collisionA_pos = vec4(0,0,0,0); // Position of collision between missile A and an asteroid
vec4 collisionB_pos = vec4(0,0,0,0); // Position of collision between missile B and an asteroid
vec4 missileB_dir; // Direction of missile B
vec4 missileB_pos; // Position of missile B
int missileA_fired = 0; // Boolean variable to check whether missile A has been fired
int missileB_fired = 0; // Boolean variable to check whether missile B has been fired
float missileA_speed = 0; // Speed of missile A
float missileB_speed = 0; // Speed of missile B
float explosion_scale_factA = 0; // Scale factor to simulate the explosion (scale explosion by this value)
float explosion_scale_factB = 0; // Scale factor to simulate the explosion (scale explosion by this value)

// The ammo box and the missiles are placed (to give user more ammunition)
vec4 missile_pos = vec4(randRange(-100,100),0,randRange(-100,100),1); // Random position of the missile supply 
vec4 ammo_box(randRange(-50,50),0,randRange(-50,50),0); // Random position of the ammo box

// END - FIELDS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Helper Functions

// Returns the length of a vector
float vec_length(vec4 vec)
{
	return sqrt(vec.x*vec.x + vec.z*vec.z);
}

// Detect a collision given two positions and a distance between the two
bool detect_collision(vec4 pos_A, vec4 pos_B, float distance)
{
	float v_x = pos_A.x - pos_B.x;
	float v_y = pos_A.y - pos_B.y;
	float v_z = pos_A.z - pos_B.z;
	if(sqrt(v_x*v_x + v_y*v_y + v_z*v_z) < distance)
		return true;
	else
		return false;
}

// Coordinates the rotation of the ship so the ship "rebalances itself"
float coordinateShipTurn(float turn_rot)
{
	turn_rot = fmodf(turn_rot,360);

	if((turn_rot > 0 && turn_rot < 90) || (turn_rot > 180 && turn_rot < 270))
		turn_rot -= 0.5;

	else if((turn_rot > 270 && turn_rot < 360) || (turn_rot > 90 && turn_rot < 180))
		turn_rot += 0.5;

	else if((turn_rot < 0 && turn_rot > -90) || (turn_rot > -270 && turn_rot < -180))
		turn_rot += 0.5;

	else if((turn_rot < -360 && turn_rot > -270) || (turn_rot < -90 && turn_rot > -180))
		turn_rot -= 0.5;

	return turn_rot;
}


// END - Helper Functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Display Functions - The following methods are responsible
// for calculatting and displaying..
// i. The bullets remaining (on the GUI)
// ii. The lives remaining (also on the GUI)
// iii. Coordinating the movement and displaying of the asteroids
// iv. Displaying the particle trail behind the ship
// v. Displaying the missiles
// vi. Displaying the bullets

// i. Display the amount of bullets remaining along the top of the screen
void display_bullets_rem(float uniformId_modelView, mat4 Projection)
{
	// Display simple wire cubes along the top of the screen for each bullet
	float offset = 0;
	for(int i = 0; i < playerA.bullets_remaining ; i++)
	{
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, Translate(-0.9 + offset,0.9, 0) * Scale(0.002,0.01,0.0) * Projection);
		displayWireCube(vec4(1,0,0,1));

		offset += 0.03;
	}
}

// ii. Display the amount of lives remaining along the top of the screen
void display_lives_rem(float uniformId_modelView, mat4 Projection)
{
	float temp = 0;
	for(int i = 0; i < playerA.lives_remaining ; i++)
	{
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, Translate(-0.9 + temp,-0.9,0) * Scale(0.003,0.01,0.0) * Projection);
		displayWireCube(vec4(1,0,0,1));

		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, Translate(-0.9 + temp,-0.9,0) * RotateZ(90) * Scale(0.003,0.01,0.0) * Projection);
		displayWireCube(vec4(1,0,0,1));

		temp += 0.1;
	}

}

// iii. Display the asteroids in the game
void display_asteroids(float uniformId_modelView, mat4 projView)
{
	// Randomly generate asteroids on different sides of the game
	int rand_asteroid = 0;

	// If the sphere size has decreased, draw more spheres
	if(spheres.size() != num_spheres)
	{
		// Draw as many spheres as needed to maintain a constant "num_spheres" in play
		for(int i = spheres.size(); i < num_spheres; i++)
		{
			rand_asteroid = rand() % 4;

			if(rand_asteroid == 0)
			{
				sphere_size.push_back(1);
				sphere_speed.push_back(randRange(0.05,0.15));
				sphere_angs.push_back(randRange(-50,50));
				sphere_dirs.push_back(vec4(-1,0,0,0));
				spheres.push_back(vec4(100,-1,randRange(-130,130),0));
			}
			else if(rand_asteroid == 1)
			{
				sphere_size.push_back(1);
				sphere_speed.push_back(randRange(0.05,0.15));
				sphere_angs.push_back(randRange(-50,50));
				sphere_dirs.push_back(vec4(1,0,0,0));
				spheres.push_back(vec4(-100,-1,randRange(-130,130),0));
			}
			else if(rand_asteroid == 2)
			{
				sphere_size.push_back(1);
				sphere_speed.push_back(randRange(0.05,0.15));
				sphere_angs.push_back(randRange(-50,50));
				sphere_dirs.push_back(vec4(0,0,-1,0));
				spheres.push_back(vec4(randRange(-130,130),-1,100,0));
			}
			else if(rand_asteroid == 3)
			{
				sphere_size.push_back(1);
				sphere_speed.push_back(randRange(0.05,0.15));
				sphere_angs.push_back(randRange(-50,50));
				sphere_dirs.push_back(vec4(0,0,1,0));
				spheres.push_back(vec4(randRange(-130,130),-1,-100,0));
			}
			
		}
	}
	
	// For all of the spheres in the game..
	for(int i = 0; i < spheres.size() ; i++)
	{
		// If there is a collision between the ship and a sphere..
		if(detect_collision(current_pos, spheres[i], 2))
		{
			// Decrement the lives remaining (if killed any orbs) and reposition the user
			current_pos = vec4(0,0,0,0);
			if(killed_since_death > 0)
			{
				killed_since_death = 0;
				playerA.lives_remaining--;
			}				
		}
			
		// Reposition each orb according to its rotatation, speed and direction
		spheres[i] = spheres[i] + RotateY(sphere_angs[i]) * sphere_speed[i] *sphere_dirs[i];
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, projView * Translate( spheres[i] ) * Scale(sphere_size[i], 0.5 * sphere_size[i], sphere_size[i]));
		displayWireSphere(vec4(0.545f,0.275f,0.08f,1));

		// If the sphere is growing (meaning it has been shot) continue to increment its size
		if(sphere_size[i] > 1)
		{
			sphere_size[i] += 0.1;
		}

		// If a sphere leaves the grid, erase it and all of its fields
		if( (spheres[i].x > 100) || (spheres[i].z > 100) || (spheres[i].x < -100) || (spheres[i].z < -100) || sphere_size[i] > 2)
		{
			sphere_size.erase(sphere_size.begin()+i);
			sphere_speed.erase(sphere_speed.begin()+i);
			spheres.erase(spheres.begin()+i);
			sphere_angs.erase(sphere_angs.begin()+i);
			sphere_dirs.erase(sphere_dirs.begin()+i);
		}
		
	}

}


// iv. Display the particle trail
void display_particles(float uniformId_modelView, mat4 matProj)
{
	// Draw the cylinder which seemingly "emits" the trail
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * Translate(current_pos) * Translate(0,-2,0) * RotateZ(90) * RotateX(-45) * Scale(0.3,0.3,0.3) * RotateX(angle_rot));
	displayWireCylinder(vec4(1,1,1, 0.7f));

	// If their are less than 30 particle fields, continue to add them
	if(particle_fields.size() < 30)
	{
		shipParticles temp;
		temp.init();

		particle_fields.push_back(temp);
		particle_dens.push_back(Translate(current_pos + vec4(0,-2,0,0)) * RotateZ(90) * RotateX(-45) * RotateX(angle_rot));
	}
	// If there is more than 30, add and erase a new field
	else
	{
		shipParticles temp;
		temp.init();
		particle_fields.push_back(temp);
		particle_fields.erase(particle_fields.begin());

		particle_dens.push_back(Translate(current_pos + vec4(0,-2,0,0)) * RotateZ(90) * RotateX(-45) * RotateX(angle_rot));
		particle_dens.erase(particle_dens.begin());
	}

	// Draw all of the particle fields, either red or range (depending on random integer -> (0,1))
	for(int i = 0; i < particle_fields.size() ; i++)
	{
		int colour_rand = rand() % 2;
		if(colour_rand == 0)
		{
			glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * particle_dens[i]);
			glUniform4fv(uniformId_colour, 1, vec4(1,0.5,0, 0.7f));
			particle_fields[i].draw(true);
		}
		else
		{
			glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * particle_dens[i]);
			glUniform4fv(uniformId_colour, 1, vec4(1,0.2,0, 0.7f));
			particle_fields[i].draw(true);
		}
	}
}


// v. Display the missiles
void display_missiles(float uniformId_modelView, mat4 matProj)
{
	// If there is a collision for missile A
	if(abs(collisionA_pos.x) > 0.1 && abs(collisionA_pos.z) > 0.1 && explosion_scale_factA > 0 && explosion_scale_factA < 10)
	{
		// Draw the explosion and increase its size for the next iteration
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * Translate(collisionA_pos) * Scale(explosion_scale_factA));
		explosion_scale_factA += 0.5;
		displayWireSphere(vec4(0.8,0.2,0, 0.7f));
	}
	// If there is a collison for missile B
	if(abs(collisionB_pos.x) > 0.1 && abs(collisionB_pos.z) > 0.1 && explosion_scale_factB > 0 && explosion_scale_factB < 10)
	{
		// Draw the explosion and increase its size for the next iteration
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * Translate(collisionB_pos) * Scale(explosion_scale_factB));
		explosion_scale_factB += 0.5;
		displayWireSphere(vec4(0.8,0.2,0, 0.3f));
	}

	// - - - - - - - - - - - - - - - - - - - - 
	// Draw missile A
	mat4 missileA_loc;
	// If the missile has no speed, then it is simply a part of the ship
	if(missileA_speed == 0)
	{
		missileA_loc = Translate(current_pos) * Translate(0,-2,0) * Scale(0.3,0.3,0.3) * RotateY(angle_rot-45) * RotateX(turn_rot) * RotateZ(90) ;
	}
	// If it does have speed..
	else
	{
		// Calculate its position, and check to see if it collides with any asteroids
		missileA_pos = missileA_pos + missileA_speed*missileA_dir;
		missileA_loc = Translate(missileA_pos) * Translate(0,-2,0) * Scale(0.3,0.3,0.3) * RotateY(angle_rot-45) * RotateX(turn_rot) * RotateZ(90) ;
		for(int i = 0; i < spheres.size() ; i++)
		{
			// If the missile does collide with an asteroid, increment the kill counter and update positions
			if(detect_collision(missileA_pos, spheres[i], 2) && missileA_fired == 0)
			{
				killed_since_death++;
				explosion_scale_factA += 0.3;
				collisionA_pos = missileA_pos;
				missileA_fired = 1;
			}
		}
	}
	// If the distance between the missile and the current position  is less than 250 (AKA in the grid)
	if(vec_length(missileA_pos - current_pos) < 250 && missileA_fired == 0)
	{
		// Draw the cylinder for the missile
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * missileA_loc * Translate(vec4(0,0,2,1)) * Scale(0.5,1.0,0.5));
		glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
		displayWireCylinder(vec4(0.3,0.3,0.3,1));

		// Draw the sphere for the missile
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * missileA_loc * Translate(vec4(0,-0.5,2,1))  * Scale(0.25,0.25,0.25));
		glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
		displayWireSphere(vec4(0.3,0.3,0.3,1));
	}
	// If it exceeds the distance, then set the boolean variable to 1 to indicate it has been fired
	else 
	{
		missileA_fired = 1;
	}
	// If there is a collision position..
	if(abs(collisionA_pos.x) > 0.1 && abs(collisionA_pos.z) > 0.1)
	{
		// Find all of the spheres within range of the explosion 
		for(int i =0; i < spheres.size(); i++)
		{
			if(vec_length(spheres[i] - collisionA_pos) < 15)
			{
				sphere_size[i] += 0.1;
			}
		}
	}
	// END - missile A - - - - - - - - - - - -


	// - - - - - - - - - - - - - - - - - - - - 
	// Draw missile B
	mat4 missileB_loc;
	// If the missile has no speed, then it is simply a part of the ship
	if(missileB_speed == 0)
	{
		missileB_loc = Translate(current_pos) * Translate(0,-2,0) * Scale(0.3,0.3,0.3) * RotateY(angle_rot-45) * RotateX(turn_rot) * RotateZ(90) ;
	}
	// If it does have speed..
	else
	{
		// Calculate its position, and check to see if it collides with any asteroids
		missileB_pos = missileB_pos + missileB_speed*missileB_dir;
		missileB_loc = Translate(missileB_pos) * Translate(0,-2,0) * Scale(0.3,0.3,0.3) * RotateY(angle_rot-45) * RotateX(turn_rot) * RotateZ(90) ;
		for(int i = 0; i < spheres.size() ; i++)
		{
			// If the missile does collide with an asteroid, increment the kill counter and update positions
			if(detect_collision(missileB_pos, spheres[i], 2) && missileB_fired == 0)
			{
				killed_since_death++;
				explosion_scale_factB += 0.3;
				collisionB_pos = missileB_pos;
				missileB_fired = 1;
			}
		}
	}
	// If the distance between the missile and the current position  is less than 250 (AKA in the grid)
	if(vec_length(missileB_pos - current_pos) < 140  && missileB_fired == 0)
	{
		// missile 1
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * missileB_loc * Translate(vec4(0,0,-2,1)) * Scale(0.5,1.0,0.5));
		glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
		displayWireCylinder(vec4(0.3,0.3,0.3,1));

		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * missileB_loc * Translate(vec4(0,-0.5,-2,1))  * Scale(0.25,0.25,0.25));
		glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
		displayWireSphere(vec4(0.3,0.3,0.3,1));

	}
	// If it exceeds the distance, then set the boolean variable to 1 to indicate it has been fired
	else
	{
		missileB_fired = 1;
	}
	// If there is a collision position..
	if(abs(collisionB_pos.x) > 0.1 && abs(collisionB_pos.z) > 0.1)
	{
		// Find all of the spheres within range of the explosion 
		for(int i =0; i < spheres.size(); i++)
		{
			if(vec_length(spheres[i] - collisionB_pos) < 15)
			{
				sphere_size[i] += 0.1;
			}
		}
	}
	// END - missile B - - - - - - - - - - - -

	// Draw the randomly placed missile which when touched refulls the missiles
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj  * Translate(missile_pos) * Translate(vec4(0,0,2,1)) * Scale(0.5,1.0,0.5));
	glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
	displayWireCylinder(vec4(0.3,0.3,0.3,1));
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj  * Translate(missile_pos) * Translate(vec4(0,-0.5,2,1))  * Scale(0.25,0.25,0.25));
	glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
	displayWireSphere(vec4(0.3,0.3,0.3,1));
	
	// If the user finds the random missile, reset the missile information and replace the missile
	if(detect_collision(current_pos, missile_pos, 1.5))
	{
		missileA_speed = 0;
		missileA_dir = vec4(0,0,0,0);
		missileA_pos = vec4(0,0,0,0);
		missileA_fired = 0;
		missileB_speed = 0;
		missileB_dir = vec4(0,0,0,0);
		missileB_pos = vec4(0,0,0,0);
		missileB_fired = 0;
		collisionA_pos = vec4(0,0,0,0);
		collisionB_pos = vec4(0,0,0,0);
		explosion_scale_factA = 0;
		explosion_scale_factB = 0;
		
		missile_pos = vec4(randRange(-100,100),-2,randRange(-100,100),1);
	}
	
}

// vi. Display the bullets
void display_bullets(float uniformId_modelView, mat4 matProj)
{
	// If a bullet is fired, and the player has bullets remaining
	if(bullet_fired && playerA.bullets_remaining > 0)
	{
		// Reverse the boolean, and add the current position and direction of the bullet
		bullet_fired = !bullet_fired;
		bullet_positions.push_back(current_pos + vec4(0,-1,0,0));
		bullet_directions.push_back( RotateY(angle_rot) * vec4(1,0,1,0));
		playerA.bullets_remaining--;
	}

	// Draw the random box to allow the player to gain ammunition
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * Translate(ammo_box) * Translate(0,-1,0) );
	displayWireCube(vec4(0.1,0.8,0.1,1));

	// If the user touches the ammo box, replace the ammo box randomly and give the player more ammunition
	if(detect_collision(current_pos, ammo_box, 0.9))
	{
		ammo_box = vec4(randRange(-50,50),0,randRange(-50,50),0);
		if(playerA.bullets_remaining < 46)
		{
			playerA.bullets_remaining += 15;			
		}
	}

	// For all of the bullets fired and within range..
	for(int i = 0 ; i < bullet_positions.size() ; i++)
	{
		// Translate the bullet position and draw the bullet
		glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, matProj * Translate(bullet_positions[i] + bullet_directions[i]) * Scale(0.05,0.05,0.05) );
		bullet_positions[i] = bullet_positions[i] + bullet_directions[i]*2.5;
		displayWireSphere(vec4(1.0f,0.0f,0.0f,1));

		for(int m = 0; m < spheres.size(); m++)
		{
			// If the bullet collides with an asteroid, increase its scale value (size)
			if(detect_collision(spheres[m] , bullet_positions[i], 1.8f))
			{
				sphere_size[m] += 0.05;
			}
		}
		
		// If the bullet moves too far away, erase it
		if(bullet_positions[i].x > 140 || bullet_positions[i].z > 140 || bullet_positions[i].x < -140 || bullet_positions[i].z < -140)
		{
			bullet_positions.erase(bullet_positions.begin()+i);
			bullet_directions.erase(bullet_directions.begin()+i);
		}
	}
}

// Display method 
void display( void )
{
	 // clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate the project and view matrices
	mat4 Projection  = myCamera.getProjection();
	mat4 View = myCamera.getView() * RotateY(-angle_rot) * Translate(-current_pos);


	// Draw the ship
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, Projection * View * Translate(current_pos) * Translate(0,-2,0) * Scale(0.3,0.3,0.3) * RotateY(angle_rot-45) * RotateX(turn_rot));
	glUniform4fv(uniformId_colour, 1, vec4(1,1,1, 0.7f));
	ship.draw();


	// Display the missiles
	display_missiles(uniformId_modelView, Projection * View);

	// Display the particle trail
	display_particles(uniformId_modelView, Projection * View);
	
	// Update the current position
	current_pos = current_pos + current_dir * speed;
	turn_rot = coordinateShipTurn(turn_rot);
	if(speed < max_speed)
		speed += 0.002;

	// Display the bullets
	display_bullets(uniformId_modelView, Projection * View);
	
	// Display the asteroids
	display_asteroids(uniformId_modelView, Projection * View);

	// Display the stars
	glUniformMatrix4fv(uniformId_modelView, 1, GL_TRUE, Projection * View	);
	displayStars(vec4(1,1,1,1));

	// Display the amount of bullets and lives remaining
	display_lives_rem(uniformId_modelView, Projection);
	display_bullets_rem(uniformId_modelView, Projection);

	// swap buffers and display
	glutSwapBuffers();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void reshape(int width, int height)
{
	if(width>height)
	{
		glViewport((width-height)/2, 0, std::min(width,height), std::min(width,height));
	}
	else
	{
		glViewport(0, (height-width)/2, std::min(width,height), std::min(width,height));
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// keyboard event callback function
bool fillAsLines = false;
int lineWidth = 1;
int winding = 0;
bool isCulling = false;


void keyboard( unsigned char key, int x, int y )
{
	switch ( key )
	{
		// Esc key
		case 033:
			exit( EXIT_SUCCESS );
			break;

		case ' ':

			bullet_fired = ! bullet_fired;
			break;

		case '.':
			myCamera.toggleTopView();
			break;

		case 'a':
			speed += 0.03;
			break;
		case 'A':
			speed += 0.03;
			break;
		case 's':
				speed -= 0.03;
			break;
		case 'S':
				speed -= 0.03;
			break;
		case 'z':
			
			missileA_pos = current_pos;
			missileA_dir = RotateY(angle_rot) * vec4(1,0,1,0);
			missileA_speed = 0.8;
			break;
		case 'x':
			
			missileB_pos = current_pos;
			missileB_dir = RotateY(angle_rot) * vec4(1,0,1,0);
			missileB_speed = 0.8;
			break;
	}

	adjustable.key(key, x, y);
	glutPostRedisplay();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// non ASCII keyboard event callback
void specialKeyboard(int key, int x, int y)
{
	std::string result = adjustable.specialKey(key, x, y);

	updateCamera();

	switch ( key )
	{
		// Esc key
		case GLUT_KEY_UP:
			current_dir = initial_dir;
			break;

		case GLUT_KEY_DOWN:
			break;

		case GLUT_KEY_LEFT:
			turn_rot -= 3;
			angle_rot += 3;
			dir_vec = RotateY(3) * dir_vec;
			initial_dir = RotateY(3) * initial_dir;
			break;

		case GLUT_KEY_RIGHT:
			turn_rot += 3;
			angle_rot -= 3;
			dir_vec = RotateY(-3) * dir_vec;
			initial_dir = RotateY(-3) * initial_dir;
			break;
	}
	glutPostRedisplay();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

unsigned frameCount = 0;
int period = 17;
void timerFunction(int value)
{
	// first time only
	if (0 != value)
	{
		std::ostringstream ss;
		ss << frameCount * (1000.0 / (double)period) << " FPS";
		glutSetWindowTitle(ss.str().c_str());
	}
	else
	{
		std::cout << "timerFunction first call" << std::endl;
	}
	frameCount = 0;

	glutPostRedisplay();
	glutTimerFunc(period, timerFunction, 1);
}


// application entry point
int	main( int argc, char **argv )
{
	// initialize glut
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize( 512, 512 );

	#ifndef __APPLE__
	// If you are using freeglut extensions, ask for an OpenGL
	// version context
    glutInitContextVersion( 2, 1 );
	// this seems to force the most recent version, but would be nice
	// to call since it forces modern OpenGL calls
	//glutInitContextFlags (GLUT_FORWARD_COMPATIBLE );
    //glutInitContextProfile( GLUT_CORE_PROFILE );
	#endif
	// create the window
	glutCreateWindow( "Demo" );

	#ifndef __APPLE__
	// initialize the OpenGL extension wrangler
	glewExperimental = GL_TRUE; // seems to help with older contexts
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		return 1;
	}
	#endif

	// query for version
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	// set window background colour
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// blending and smoothing
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	// load shaders into the GPU
	initShaders();

	// create geometry and put it into the GPU
	initGeometry();


	// set event callback functions
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( specialKeyboard );


	updateCamera();

	playerA.bullets_remaining = 60;
	playerA.lives_remaining = 4;

	std::cout << "initializing done" << std::endl;


	glutTimerFunc(17, timerFunction, 0);
	// enter the main loop
	glutMainLoop();

	// should never get to this point
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
