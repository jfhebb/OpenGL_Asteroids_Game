#ifndef ADJUSTABLE_H_
#define ADJUSTABLE_H_
/*
 *  Simple way to adjust variables for demos.
 *
 */

#include "gl_include.h"

#include <vector>
#include <string>

namespace djv
{


//class AdjustableVariableBase
//{

//
//};

//template <class T>
class AdjustableVariable 
{
public:

	AdjustableVariable(GLfloat* v, unsigned char k, std::string d, 
					   GLfloat sa, GLfloat la)
	{
		var = v;
		key = k;
		desc = d;
		smallAdjust = sa;
		largeAdjust = la;
	}

	GLfloat* var;
	std::string desc;
	unsigned char key;
	GLfloat smallAdjust;
	GLfloat largeAdjust;

};

class Adjustable
{
public:
	Adjustable();
	virtual ~Adjustable();

	void add(GLfloat* a, unsigned char key, char* desc, 
			 GLfloat smallAdjust = 0.1f, GLfloat largeAdjust = 1.0f);

	void key(unsigned char key, int x, int y);
	std::string specialKey(int key, int x, int y);

private:

	std::vector< AdjustableVariable > mVars;
	unsigned int mCurrent;

};




}

#endif /* ADJUSTABLE_H_ */
