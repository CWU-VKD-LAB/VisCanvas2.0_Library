#include "Class.h"
#include "Color.h"


// create a class at index -1 with a "-1" string for a name
Class::Class() {
	this->index = -1;
	this->name = "-1";
	this->color = Color();
	points = std::vector<int>();
}

// create a class at index(index) and the passed string for a name(name)
Class::Class(int index, std::string name) {
	this->index = index;
	this->name = name;
	this->color = Color();
	points = std::vector<int>();
}

// delete the object
Class::~Class() {
}


// gets the index of the class
int Class::getIndex() {
	return this->index;
}

// sets the index of the class, returns previous index
int Class::setIndex(int newIndex) {
	int previousIndex = getIndex();
	index = newIndex;
	return previousIndex;
}

// gets a copy of the color components for this class
std::vector<double>* Class::getColor() {
	return color.getColorComponents();
}

// sets the color for this class
void Class::setColor(std::vector<double>& newColor) {
	if (newColor.size() != 4) {
		return;
	}
	color.setRed(newColor[0]);
	color.setGreen(newColor[1]);
	color.setBlue(newColor[2]);
	color.setAlpha(newColor[3]);
}

// sets the color for this class
void Class::setColor(std::vector<double>* newColor) {
	if (newColor == nullptr) {
		return;
	}
	if (newColor->size() != 4) {
		return;
	}
	color.setRed((*newColor)[0]);
	color.setGreen((*newColor)[1]);
	color.setBlue((*newColor)[2]);
	color.setAlpha((*newColor)[3]);
}

// gets the name of the class
std::string * Class::getName() {
	return &(this->name);
}

// sets the name of the class, returns previous name
void Class::setName(std::string * newName) {
	name = *newName;
}



// gets the number of sets in this class
int Class::getPointNumber() const
{
	return points.size();
}

// gets the sets in the class
std::vector<int>* Class::getPointsInClass() {
	return &points;
}

// gets the sets in the class
void Class::addPoint(int pointIndex){
	// check if set is in class
	for (int i = 0; i < points.size(); i++) {
		if (points[i] == pointIndex) {
			return;
		}
	}
	points.push_back(pointIndex);
}

// removes set from the class
void Class::removePoint(int pointIndex) {
	for (int i = 0; i < points.size(); i++) {
		if (points[i] == pointIndex) {
			points.erase(points.begin() + i);
			return;
		}
	}
}
