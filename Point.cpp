#include "Point.h"

// create a point for index -1 and class -1
Point::Point()
{
	currentIndex = -1;
	originalIndex = -1;
	pointClass = 0;
	name = "-1";
	visible = true;
}

// create a point for the pass index(index) and passed size(size)
Point::Point(int pointIndex, int classIndex)
{
	currentIndex = pointIndex;
	originalIndex = pointIndex;
	pointClass = classIndex;
	name = std::to_string(pointIndex);
	visible = true;
}

// delete the object
Point::~Point() {
}

// gets the class(data class) of this point
int Point::getClass() const {
	return pointClass;
}

// sets the class(data class) of this point and returns the previous class
int Point::setDataClass(int newClass) {
	int previousClass = pointClass;
	pointClass = newClass;
	return previousClass;
}

// gets the index of the point
int Point::getIndex() const {
	return currentIndex;
}

// sets the index of the point, returns previous index
int Point::setIndex(int newIndex) {
	int previousIndex = currentIndex;
	currentIndex = newIndex;
	return previousIndex;
}

// gets the original index of the point
int Point::getOriginalIndex() const {
	return originalIndex;
}

// gets the name of the point
std::string* Point::getName() {
	return &(this->name);
}

// sets the name returns previous name
void Point::setName(std::string &newName) {
	name = newName;
}

// checks if the point is visible or not
bool Point::isVisible() {
	return visible;
}

// sets the point visible or not visible
bool Point::setVisible(bool newVisible) {
	bool oldVisibility = visible;
	visible = newVisible;
	return oldVisibility;
}
