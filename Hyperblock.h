/**
 * A class to model hyperblocks.
 * It includes containers for the points and block and functions to manipulate them.
 */

#pragma once
#include "Point.h"
#include "Coordinate.h"
#include "Color.h"

#include <vector>
#include <string> 

class Hyperblock {
public:

	Hyperblock();
	// creates a blank block
	Hyperblock(Color &blockColor);
	// creates the block with the passed points
	Hyperblock(Color &blockColor, std::vector<int>* newPointsInBlock);
	// creates the block with the passed points and values from the passed coordinates
	Hyperblock(Color &blockColor, std::vector<int>* newPointsInBlock, std::vector<Coordinate*>* coordinatesToCalculateWith);
	// deletes object
	~Hyperblock();

	// gets the calculates the minimum value of coordinate for the points whose indexes are passed(pointIndexes)
	static double getMinimumValue(Coordinate* coordinate, std::vector<int>* pointIndexes);
	// gets the calculates the mean value of coordinate for the points whose indexes are passed(pointIndexes)
	static double getMeanValue(Coordinate* coordinate, std::vector<int>* pointIndexes);
	// gets the calculates the median value of coordinate for the points whose indexes are passed(pointIndexes)
	static double getMedianValue(Coordinate* coordinate, std::vector<int>* pointIndexes);
	// gets the calculates the maximum value of coordinate for the points whose indexes are passed(pointIndexes)
	static double getMaximumValue(Coordinate* coordinate, std::vector<int>* pointIndexes);
	// gets the center point of the block
	vector<double> getCenter();

	// adds the passed point index(pointIndex) to the list of points
	// returns true if the point is added and false if the point was already in the block
	bool addPoint(int pointIndex);
	// removes the point index(pointIndex) to the list of points
	// returns true if the point is removed and false if the point not in the block
	bool removePoint(int pointIndex);
	// gets the number of points in the block
	int getPointNumber();
	// gets the index of the point in this block for the point at the passed index(blockRelativeIndex), which is the index the point index is stored at
	int getIndexOfPoint(int blockRelativeIndex);

	// gets the minimum value in the block for the coordinate at the passed index 
	double getMinimum(int coordinateIndex) const;
	// gets the mean value in the block for the coordinate at the passed index 
	double getMiddle(int coordinateIndex) const;
	// gets the maximum value in the block for the coordinate at the passed index 
	double getMaximum(int coordinateIndex) const;

	// recalculates the values for the block using the passed coordinate
	void calculateValues(std::vector<Coordinate*>* coordinateToCalculateWith);
	// inverts the values of the point at the passed index
	void invertValues(int coordinateToInvertValuesAt);
	// move the position of the values in the point(at originalIndex) to the index after indexBeforeNewIndex
	bool moveValues(int originalIndex, int indexOfInsertion);

	int getSize() const;

	// gets the color components of the block
	std::vector<double>* getColor();
	// sets the color components of the block
	void setColor(std::vector<double>& newColor);
	// sets the color components of the block
	void setColor(std::vector<double>* newColor);

	// gets the radius of this block
	double getRadius();
	// sets the radius of this block and returns the old one
	double setRadius(double newRadius);

	// gets the original point of this block
	int getOriginalPoint();
	// sets the original point of this block and returns the old one
	int setOriginalPoint(int newPoint);

	// gets the name of this block
	std::string* getName();
	// sets the name of this block
	void setName(std::string* newName);
	// gets the points in the block
	std::vector<int>* getPoints();

	// get whether the the block uses the mean or median
	bool isUseMean();
	// set whether the the block uses the mean or median
	void setUseMean(bool newUseMean);

	// get whether the block is displayed
	bool isDisplayed();
	// set whether the block is displayed
	void setDisplayed(bool displayed);

	vector<double> getVirtualCenter(int numOfDimensions);
	double getRatio(int numOfDimensions);

private:
	Color color;
	std::vector<int> pointsInBlock;
	std::vector<double> minimumValues;
	std::vector<double> meanValues;
	std::vector<double> medianValues;
	std::vector<double> maximumValues;

	int size;
	double radius;
	int originalPoint;
	bool useMean;
	bool displayed;
	std::string name;


};
