/**
 * A class that holds all data related to a class of a dataset.
 * This includes the classes name, points, color, etc.
 */

#ifndef Class_H
#define Class_H
#include "Color.h"
#include <string>
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


// a class to hold and manage data with a common index for several points
class Class {
public:
	// create a class at index -1 with a "-1" string for a name
	Class();
	// create a class at index(index) and the passed string for a name(name)
	Class(int index, std::string name);
	// delete the object
	~Class();
	// gets the index of the class
	int getIndex();
	// sets the index of the class, returns previous index
	int setIndex(int newIndex);
	// gets a copy of the color components for this class
	std::vector<double>* getColor();
	// sets the color for this class
	void setColor(std::vector<double> &newColor);
	// sets the color for this class
	void setColor(std::vector<double>* newColor);
	// gets the name of the class
	std::string* getName();
	// sets the name of the class, returns previous name
	void setName(std::string* newName);
	// gets the number of points in this class
	int getPointNumber() const;
	// gets the points in the class
	std::vector<int>* getPointsInClass();
	// adds point to the class
	void addPoint(int pointIndex);
	// removes point from the class
	void removePoint(int pointIndex);

private:
	// the vector holding all the data
	int index;
	// gets the color data of this class should be painted
	Color color;
	// gets the name of this class
	std::string name;
	//a field holding the points in this data class
	std::vector<int> points;
};

#endif