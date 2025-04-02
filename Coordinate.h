/**
 * A class that holds all the data for a single coordinate.
 * This includes the coordinates name, data, whether its shifted, inverted, operations, etc.
 */

#ifndef Coordinate_H
#define Coordinate_H

#include "Node.h"
#include <vector>
#include <string>

/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


using namespace std;

// a class to hold and manage data with a common index for several points
class Coordinate {
public:
	// create a coordinate of size 0 at index 0
	Coordinate();
	// create a coordinate for the pass index(index) and passed size(size)
	Coordinate(int index, int size);
	// delete the object
	~Coordinate();

	// get the index of the the coordinate was created with
	int getOriginalIndex() const;
	// set the index of the the coordinate, and returns the old index
	int setOriginalIndex(int newIndex);
	// calibrate the data to the [0,1] space
	void calibrateData();
	// gets the data for the point of the passed index(dataIndex)
	double getData(int dataIndex) const;
	// gets the data calibrated, but not inverted or shifted data for the point of the passed index(dataIndex)
	double getCalibratedData(int dataIndex) const;
	// gets the original data for the point of the passed index(dataIndex)
	double getOriginalData(int dataIndex) const;
	// gets the name
	string* getName();
	// sets the name and returns the old name
	void setName(string* newName);
	// sets the data of the point at the passed index(dataIndex) to the passed value(newData), alters the original data
	void setData(int dataIndex, double newData);


	// multiplies all the data in the coordinate by the passed double, does not alter original data
	void multiplyData(double multiplier);
	// divides all the data in the coordinate by the passed double, does not alter original data
	void divideData(double divisor);
	// adds the passed double to all the data in the coordinate, does not alter original data
	void addToData(double addend);
	// adds the amount passed(shiftAmount) to the data shift amount
	void shiftDataBy(double modToShiftAmount);
	// gets the amount data shift amount
	double getShift();
	// returns whether the dimension is inverted or not
	bool isInverted();
	// toggles whether the data is inverted
	void invert();


	// gets the number of points in the coordinate
	int size() const;
	// returns whether there is artificial calibration or not
	bool isArtificiallyCalibrated();
	// sets the calibration to use the data's(not the artificial) maximum and minimum
	void clearArtificialCalibration();
	// sets the bounds to be used for artificial calibration
	void setCalibrationBounds(double newMaximum, double newMinimum);
	// gets the artificial maximum
	double getArtificialMaximum() const;
	// gets the artificial minimum
	double getArtificialMinimum() const;
	// gets the maximum data value in the coordinate
	double getMaximum() const;
	// gets the minimum data value in the coordinate
	double getMinimum() const;

	// gets all the data in the coordinate
	vector<Node*> getAllData();
	// gets whether this coordinate is drawn or not
	bool isVisible();
	// sets whether this coordinate is drawn or not
	void setVisibility(bool isDrawn);

private:
	// the vector holding all the data
	vector<Node*> data;
	// the amount to add to data when it is retrieved
	double shiftAmount;
	// holds whether the data is inverted or not
	bool inverted;
	// holds whether the coordinate is drawn or not
	bool isDrawn;
	// gets the index the coordinate was originally at
	int originalIndex;
	// the boolean checking whether to use artificial calibration
	bool useArtificialCalibration;
	// the maximum to calibrate the set with
	double artificialMaximum;
	// the minimum to calibrate the set with
	double artificialMinimum;
	// the name of the dimension
	std::string name;
};

#endif