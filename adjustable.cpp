/*
 * Adjustable.cpp
 *
 *  Created on: Jan 18, 2010
 *      Author: Dan
 */

#include "Adjustable.h"


#include <stdlib.h>
#include <iostream>


using namespace std;

namespace djv
{

Adjustable::Adjustable()
{
	// TODO Auto-generated constructor stub

}

Adjustable::~Adjustable()
{
	// TODO Auto-generated destructor stub
}

void Adjustable::add(GLfloat* a, unsigned char key, char* desc,
					 GLfloat smallAdjust, GLfloat largeAdjust)
{

	mVars.push_back(AdjustableVariable(a, key, desc, smallAdjust, largeAdjust));

}

void Adjustable::key(unsigned char key, int x, int y)
{
	//std::cout << key << std::endl;
	unsigned int pick = mCurrent;
	for (unsigned int i = 0; i < mVars.size(); i++)
	{
		if (mVars[i].key == key)
		{
			pick = i;
		}
	}

	if (pick != mCurrent)
	{
		mCurrent = pick;
		std::cout << "Pick: " << mVars[mCurrent].desc << std::endl;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


std::string Adjustable::specialKey(int key, int x, int y)
{

	if (mVars.size() > 0)
	{

		GLfloat d = mVars[mCurrent].smallAdjust;
		int mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_SHIFT)
			d = mVars[mCurrent].largeAdjust;

		bool changed = false;

		switch(key)
		{
			case GLUT_KEY_UP:
				*(mVars[mCurrent].var) += d;
				changed = true;
				break;
			case GLUT_KEY_DOWN:
				*(mVars[mCurrent].var) -= d;
				changed = true;
				break;
			default:
				break;
		}

		// if a variable changed, return the name
		if (changed)
		{
			std::cout << "Set: " << mVars[mCurrent].desc <<
						" = " << *(mVars[mCurrent].var) << std::endl;
			return mVars[mCurrent].desc;
		}

	}

	// nothing happened, return null string
	return std::string();

}

}
