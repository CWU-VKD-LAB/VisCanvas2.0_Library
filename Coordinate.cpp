#include "Coordinate.h"
#include "Node.h"
#include <vector>
#include <string>

using namespace std;

// create a coordinate of size 0 at index 0
Coordinate::Coordinate() {
	originalIndex = 0;
	name = std::to_string(originalIndex);
	data = vector<Node*>();
	shiftAmount = 0.0;
	inverted = false;
	isDrawn = true;

	useArtificialCalibration = false;
	artificialMaximum = 1.0;
	artificialMinimum = 0.0;
}

// create a coordinate for the pass index(index) and passed size(size)
Coordinate::Coordinate(int index, int size) {
	originalIndex = index;

	name = std::to_string(index);

	data = vector<Node*>();
	for (int i = 0; i < size; i++) {
		data.push_back(new Node(0.0));
	}
	shiftAmount = 0.0;
	inverted = false;
	isDrawn = true;

	useArtificialCalibration = false;
	artificialMaximum = 1.0;
	artificialMinimum = 0.0;
}

// delete the object
Coordinate::~Coordinate() {
	for (unsigned int i = 0; i < data.size(); i++) {
		delete data[i];
	}
	data.clear();
}

// get the index of the the coordinate was created with
int Coordinate::getOriginalIndex() const {
	return originalIndex;
}

// set the index of the the coordinate, and returns the old index
int Coordinate::setOriginalIndex(int newIndex) {
	int oldIndex = originalIndex;
	originalIndex = newIndex;
	return oldIndex;
}

vector<Node*> Coordinate::getAllData() {
	return this->data;
}

bool Coordinate::isVisible() {
	return this->isDrawn;
}

void Coordinate::setVisibility(bool isDrawn) {
	this->isDrawn = isDrawn;
}

// calibrate the data to the [0,1] space
void Coordinate::calibrateData() {
	// reset data
	for (int i = 0; i < this->size(); i++) {
		(data[i])->resetData();
	}

	double maximum = getMaximum();
	double minimum = getMinimum();
	if (useArtificialCalibration) {
		maximum = artificialMaximum;
		minimum = artificialMinimum;
	}
	double range = maximum - minimum;
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).setData(((*data[i]).getData() - minimum) / range);
		//(*data[i]).addToData(-minimum);
		//(*data[i]).divideData(range);
	}
}

// gets the data for the point of the passed index(dataIndex)
double Coordinate::getData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	double dataReturn = (*data[dataIndex]).getData();
	if (inverted) {
		dataReturn = 1 - dataReturn;
	}
	dataReturn += shiftAmount;
	return dataReturn;
}

// gets the data calibrated, but not inverted or shifted data for the point of the passed index(dataIndex)
double Coordinate::getCalibratedData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	double dataReturn = (*data[dataIndex]).getData();
	if (inverted) {
		dataReturn = 1 - dataReturn;
	}
	return dataReturn;
}

// gets the original data for the point of the passed index(dataIndex)
double Coordinate::getOriginalData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	return (*data[dataIndex]).getOriginalData();
}

// gets the name
string * Coordinate::getName() {
	return &name;
}


// sets the name and returns the old name
void Coordinate::setName(string * newName) {
	name = *newName;
}

// sets the data of the point at the passed index(dataIndex) to the passed value(newData), alters the original data
void Coordinate::setData(int dataIndex, double newData) {
	if (dataIndex >= size() || dataIndex < 0) {
		return;
	}
	(*data[dataIndex]).setData(newData);
}




// multiplies all the data in the coordinate by the passed double, does not alter original data
void Coordinate::multiplyData(double multiplier) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).divideData(multiplier);
	}
}

// divides all the data in the coordinate by the passed double, does not alter original data
void Coordinate::divideData(double divisor) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).divideData(divisor);
	}
}

// adds the passed double to all the data in the coordinate, does not alter original data
void Coordinate::addToData(double addend) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).addToData(addend);
	}
}

// adds the amount passed(shiftAmount) to the data shift amount
void Coordinate::shiftDataBy(double modToShiftAmount) {
	shiftAmount += modToShiftAmount;
}

// gets the amount data shift amount
double Coordinate::getShift() {
	return shiftAmount;
}

bool Coordinate::isInverted() {
	return inverted;
}

// toggles whether the data is inverted
void Coordinate::invert() {
	inverted = !inverted;
}


// gets the number of points in the coordinate
int Coordinate::size() const {
	return data.size();
}

// returns whether there is artificial calibration or not
bool Coordinate::isArtificiallyCalibrated() {
	return useArtificialCalibration;
}

// sets the calibration to use the data's(not the artificial) maximum and minimum
void Coordinate::clearArtificialCalibration() {
	useArtificialCalibration = false;
	calibrateData();
}

// sets the bounds to be used for artificial calibration
void Coordinate::setCalibrationBounds(double newMaximum, double newMinimum) {
	useArtificialCalibration = true;
	artificialMaximum = newMaximum;
	artificialMinimum = newMinimum;
	calibrateData();
}

// gets the artficial calibration maximum for the coordinate
double Coordinate::getArtificialMaximum() const {
	return artificialMaximum;
}

// gets the artficial calibration maximum for the coordinate
double Coordinate::getArtificialMinimum() const {
	return artificialMinimum;
}

// gets the maximum data value in the coordinate
double Coordinate::getMaximum() const {
	if (size() == 0) {
		return 0.0;
	}
	double maximum = (*data[0]).getData();
	for (unsigned int i = 1; i < data.size(); i++) {
		if (maximum <(*data[i]).getData()) {
			maximum = (*data[i]).getData();
		}
	}
	return maximum;
}

// gets the minimum data value in the coordinate
double Coordinate::getMinimum() const {
	if (this->size() == 0) {
		return 0.0;
	}
	double minimum = (*data[0]).getData();
	for (unsigned int i = 1; i < data.size(); i++) {
		if (minimum > (*data[i]).getData()) {
			minimum = (*data[i]).getData();
		}
	}
	return minimum;
}