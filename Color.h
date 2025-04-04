/**
 * A class to store RGBA values of a color.
 */

#ifndef Color_H
#define Color_H
#include <vector>
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


// a simple class to hold double values for color for a color(r,g,b, alpha) in a double array of size 4

class Color
{
public:
	// create the color
	Color();
	// delete the color
	~Color();
	
	// gets vector of the color components
	std::vector<double>* getColorComponents();
	// set the red color component value
	void setRed(double newValue);
	// set the green color component value
	void setGreen(double newValue);
	// set the blue color component value
	void setBlue(double newValue);
	// set the alpha component value
	void setAlpha(double newValue);
	// checks if the color is visible or not
	bool isVisible();
	// sets the color visible or not visible
	void setVisible(bool visible);

	
private:
	// the vector holding all the color components
	std::vector<double> colorComponents;
	// the vector holding a copy of the color components
	std::vector<double> copyColorComponents;
};


#endif