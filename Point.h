/**
 * A class for an n-D point of a dataset.
 * It includes the points index, name, color, class, etc.
 */

#ifndef Point_H
#define Point_H
#include "Class.h"
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/



// a class to hold and manage data with a common index for several points
class Point {
public:
	// create a point for index -1 and class -1
	Point();
	// create a point for the pass index(index) and passed size(size)
	Point(int pointIndex, int classIndex);
	// delete the object
	~Point();
	// gets the class(data class) of this point
	int getClass() const;
	// sets the class(data class) of this point and returns the previous class
	int setDataClass(int newClass);
	// gets the index of the point
	int getIndex() const;
	// sets the index of the point, returns previous index
	int setIndex(int newIndex);
	// gets the original index of the point
	int getOriginalIndex() const;
	// gets the name of the point
	std::string* getName();
	// sets the name returns previous name
	void setName(std::string &newName);
	// checks if the set is visible or not
	bool isVisible();
	// sets the point visible or not visible and returns the old visibility
	bool setVisible(bool newVisible);


private:
	// the class(data class) of this point holding all the data
	int pointClass;
	// gets the index the point was originally at
	int originalIndex;
	// gets the index the point is at
	int currentIndex;
	// the name of the point
	std::string name;
	// whether the point is visible or not
	bool visible;

};

#endif