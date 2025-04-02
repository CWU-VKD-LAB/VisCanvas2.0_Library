#include "Hyperblock.h"
#include <algorithm>    


Hyperblock::Hyperblock() {
	color = Color();
	pointsInBlock = std::vector<int>();
	minimumValues = std::vector<double>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	this->size = this->getPoints()->size();
}

// creates a blank block
Hyperblock::Hyperblock(Color &blockColor) {
	color = blockColor;
	pointsInBlock = std::vector<int>();
	minimumValues = std::vector<double>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	displayed = true;
	this->size = this->getPoints()->size();
}

// creates the block with the passed points
Hyperblock::Hyperblock(Color &blockColor, std::vector<int>* newPointsInBlock) {
	color = blockColor;
	pointsInBlock = std::vector<int>();
	minimumValues = std::vector<double>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	displayed = true;
	this->size = this->getPoints()->size();

	for (int i = 0; i < newPointsInBlock->size(); i++) {
		pointsInBlock.push_back((*newPointsInBlock)[i]);
	}
	std::sort(pointsInBlock.begin(), pointsInBlock.end());
}

// creates the block with the passed points and values from the passed coordinates
Hyperblock::Hyperblock(Color &blockColor, std::vector<int>* newPointsInBlock, std::vector<Coordinate*>* coordinatesToCalculateWith) {
	// intialize fields
	color = blockColor;
	pointsInBlock = std::vector<int>();
	minimumValues = std::vector<double>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	displayed = true;
	this->size = this->getPoints()->size();

	// put points into the block
	for (int i = 0; i < newPointsInBlock->size(); i++) {
		pointsInBlock.push_back(newPointsInBlock->at(i));
	}
	std::sort(pointsInBlock.begin(), pointsInBlock.end());
	// get the values from the coordinates
	// check if there are any coordinates passed
	calculateValues(coordinatesToCalculateWith);
}



// deletes object
Hyperblock::~Hyperblock() {
}


// gets the calculates the minimum value of coordinate for the points whose indexes are passed(pointsIndexes)
double Hyperblock::getMinimumValue(Coordinate* coordinate, std::vector<int>* pointIndexes) {
	double minimum = 0.0;
	std::sort(pointIndexes->begin(), pointIndexes->end());
	// check if the coordinate has enough points for the points in the passed vector(pointIndexes)
	if ((*pointIndexes)[pointIndexes->size() - 1] >= coordinate->size()) {
		return minimum;
	}
	// calculate the minimum
	minimum = coordinate->getCalibratedData((*pointIndexes)[0]);
	for (int i = 0; i < pointIndexes->size(); i++) {
		double dataValue = coordinate->getCalibratedData((*pointIndexes)[i]);
		if (dataValue < minimum) {
			minimum = dataValue;
		}

	}
	return minimum;
}

// gets the calculates the mean value of coordinate for the points whose indexes are passed(pointIndexes)
double Hyperblock::getMeanValue(Coordinate * coordinate, std::vector<int>* pointIndexes) {
	double sum = 0.0;
	std::sort(pointIndexes->begin(), pointIndexes->end());
	// check if the coordinate has enough points for the points in the passed vector(pointsIndexes)
	if ((*pointIndexes)[pointIndexes->size() - 1] >= coordinate->size()) {
		return sum;
	}
	// calculate the minimum
	for (int i = 0; i < pointIndexes->size(); i++) {
		double dataValue = coordinate->getCalibratedData((*pointIndexes)[i]);
		sum += dataValue;

	}
	return sum / ((double)pointIndexes->size());
}

// gets the calculates the median value of coordinate for the points whose indexes are passed(pointIndexes)
double Hyperblock::getMedianValue(Coordinate * coordinate, std::vector<int>* pointIndexes) {
	double sum = 0.0;
	std::sort(pointIndexes->begin(), pointIndexes->end());
	// check if the coordinate has enough points for the points in the passed vector(pointIndexes)
	if ((*pointIndexes)[pointIndexes->size() - 1] >= coordinate->size()||pointIndexes->size()==0) {
		return sum;
	}
	// get values
	std::vector<double> values = std::vector<double>();
	for (int i = 0; i < pointIndexes->size(); i++) {
		int currentIndex = pointIndexes->at(i);
		values.push_back(coordinate->getData(currentIndex));
	}
	std::sort(values.begin(), values.end());

	double median = 0.0;
	// odd number of points
	if (pointIndexes->size() % 2 == 1) {
		median = values[pointIndexes->size() / 2];
	}
	// even number of points
	else {
		median = values[pointIndexes->size() / 2];
		median += values[(pointIndexes->size() - 1) / 2];
		median /= 2.0;
	}

	return median;
}

// gets the calculates the maximum value of coordinate for the points whose indexes are passed(pointIndexes)
double Hyperblock::getMaximumValue(Coordinate* coordinate, std::vector<int>* pointIndexes) {
	double maximum = 0.0;
	std::sort(pointIndexes->begin(), pointIndexes->end());
	// check if the coordinate has enough points for the points in the passed vector(pointIndexes)
	if ((*pointIndexes)[pointIndexes->size() - 1] >= coordinate->size()) {
		return maximum;
	}
	// calculate the minimum
	maximum = coordinate->getCalibratedData((*pointIndexes)[0]);
	for (int i = 0; i < pointIndexes->size(); i++) {
		double dataValue = coordinate->getCalibratedData((*pointIndexes)[i]);
		if (dataValue > maximum) {
			maximum = dataValue;
		}

	}
	return maximum;
}

// adds the passed set index(pointIndex) to the list of points
// returns true if the point is added and false if the point was already in the block
bool Hyperblock::addPoint(int pointIndex) {
	for (int i = 0; i < pointsInBlock.size(); i++) {
		if (pointsInBlock[i] == pointIndex) {
			return false;
		}
	}
	pointsInBlock.push_back(pointIndex);
	std::sort(pointsInBlock.begin(), pointsInBlock.end());
	return true;
}

// removes the point index(pointIndex) to the list of pointss
// returns true if the point is removed and false if the point not in the block
bool Hyperblock::removePoint(int pointIndex) {
	for (int i = 0; i < pointsInBlock.size(); i++) {
		if (pointsInBlock[i] == pointIndex) {
			pointsInBlock.erase(pointsInBlock.begin() + i);
			return true;
		}
	}
	return false;
}

// gets the number of points in the block
int Hyperblock::getPointNumber() {
	return pointsInBlock.size();
}

// gets the index of the point in this block for the point at the passed index(blockRelativeIndex), which is the index the point index is stored at
int Hyperblock::getIndexOfPoint(int blockRelativeIndex) {
	if (blockRelativeIndex >= pointsInBlock.size() || blockRelativeIndex < 0) {
		return -1;
	}
	return pointsInBlock[blockRelativeIndex];
}




// gets the minimum value in the block for the coordinate at the passed index 
double Hyperblock::getMinimum(int coordinateIndex) const {
	if (coordinateIndex >= minimumValues.size() || coordinateIndex < 0) {
		return 0.0;
	}
	return minimumValues[coordinateIndex];
}

// gets the mean value in the block for the coordinate at the passed index 
double Hyperblock::getMiddle(int coordinateIndex) const {
	if (coordinateIndex >= meanValues.size() || coordinateIndex < 0) {
		return 0.0;
	}
	if (useMean) {
		return meanValues[coordinateIndex];
	}
	return medianValues[coordinateIndex];
}


// gets the maximum value in the block for the coordinates at the passed index 
double Hyperblock::getMaximum(int coordinateIndex) const {
	if (coordinateIndex >= maximumValues.size() || coordinateIndex < 0) {
		return 0.0;
	}
	return maximumValues[coordinateIndex];
}




// recalculates the values for the coordinates using the passed coordinates
void Hyperblock::calculateValues(std::vector<Coordinate*>* coordinatesToCalculateWith) {
	// get the values from the coordinates
	// check if there are any coordiates passed
	if (coordinatesToCalculateWith->size() <= 0) {
		// do nothing coordinats cannot be used for this block
	}
	// check if then largest point index is is within the coordinates passed
	else if (pointsInBlock[pointsInBlock.size() - 1] >= (*coordinatesToCalculateWith)[0]->size()) {
		// do nothing coordinates cannot be used for this block
	}
	else {
		minimumValues.clear();
		meanValues.clear();
		medianValues.clear();
		maximumValues.clear();


		// there are enough points in the coordinates to use the passed coordinates
		// so get the data for this block
		for (int i = 0; i < coordinatesToCalculateWith->size(); i++) {
			double min = getMinimumValue((*coordinatesToCalculateWith)[i], &pointsInBlock);
			double mean = getMeanValue((*coordinatesToCalculateWith)[i], &pointsInBlock);
			double median = getMedianValue((*coordinatesToCalculateWith)[i], &pointsInBlock);
			double max = getMaximumValue((*coordinatesToCalculateWith)[i], &pointsInBlock);
			minimumValues.push_back(min);
			meanValues.push_back(mean);
			medianValues.push_back(median);
			maximumValues.push_back(max);
		}
	}
}




// inverts the values of the set at the passed index
void Hyperblock::invertValues(int dimensionToInvertValuesAt) {
	if (dimensionToInvertValuesAt >= minimumValues.size() || dimensionToInvertValuesAt < 0) {
		return;
	}
	minimumValues[dimensionToInvertValuesAt] = 1 - minimumValues[dimensionToInvertValuesAt];
	meanValues[dimensionToInvertValuesAt] = 1 - meanValues[dimensionToInvertValuesAt];
	medianValues[dimensionToInvertValuesAt] = 1 - medianValues[dimensionToInvertValuesAt];
	maximumValues[dimensionToInvertValuesAt] = 1 - maximumValues[dimensionToInvertValuesAt];
}

// move the position of the values in the set(at originalIndex) to the index after indexBeforeNewIndex
bool Hyperblock::moveValues(int originalIndex, int indexOfInsertion) {
	if (indexOfInsertion == -1 || originalIndex == -1) {
		return false;
	}
	if (indexOfInsertion >= minimumValues.size() || originalIndex >= minimumValues.size()) {
		return false;
	}

	// this will swap the data 
	double temp = minimumValues[originalIndex];
	minimumValues[originalIndex] = minimumValues[indexOfInsertion];
	minimumValues[indexOfInsertion] = temp;
	
	temp = meanValues[originalIndex];
	meanValues[originalIndex] = meanValues[indexOfInsertion];
	meanValues[indexOfInsertion] = temp;

	temp = medianValues[originalIndex];
	medianValues[originalIndex] = medianValues[indexOfInsertion];
	medianValues[indexOfInsertion] = temp;

	temp = maximumValues[originalIndex];
	maximumValues[originalIndex] = maximumValues[indexOfInsertion];
	maximumValues[indexOfInsertion] = temp;

	return true;
}

// gets the color components of the block
std::vector<double>* Hyperblock::getColor()
{
	return color.getColorComponents();
}

std::vector<double> Hyperblock::getCenter()
{
	return medianValues;
}

// sets the color components of the block
void Hyperblock::setColor(std::vector<double>& newColor) {
	if (newColor.size() != 4) {
		return;
	}
	color.setRed(newColor[0]);
	color.setGreen(newColor[1]);
	color.setBlue(newColor[2]);
	color.setAlpha(newColor[3]);
}

// sets the color components of the block
void Hyperblock::setColor(std::vector<double>* newColor) {
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

// gets the radius of this block
double Hyperblock::getRadius() {
	return radius;
}

// sets the radius of this block and returns the old one
double Hyperblock::setRadius(double newRadius) {
	double oldRadius = radius;
	radius = newRadius;
	return radius;
}

// gets the original point of this block
int Hyperblock::getOriginalPoint() {
	return originalPoint;
}

// sets the original point of this block and returns the old one
int Hyperblock::setOriginalPoint(int newPoint) {
	int oldPoint = originalPoint;
	this->originalPoint = newPoint;
	return oldPoint;
}

// gets the name of this block
std::string* Hyperblock::getName()
{
	return &name;
}

// sets the name of this block
void Hyperblock::setName(std::string* newName) {
	name = (*newName);
}

// gets the sets in the block
std::vector<int>* Hyperblock::getPoints()
{
	return &pointsInBlock;
}

// get whether the the block uses the mean or median
bool Hyperblock::isUseMean() {
	return useMean;
}

// set whether the the block uses the mean or median
void Hyperblock::setUseMean(bool newUseMean) {
	useMean = newUseMean;
}


// get whether the block is displayed
bool Hyperblock::isDisplayed() {
	return displayed;
}

// set whether the block is displayed
void Hyperblock::setDisplayed(bool displayed) {
	this->displayed = displayed;
}

vector<double> Hyperblock::getVirtualCenter(int numOfCoordinates) {
	vector<double> center = vector<double>();

	for (int i = 0; i < numOfCoordinates; i++)
	{
		double localMax = getMaximum(i);
		double localMin = getMinimum(i);

		double localCenter = localMin + ((localMax - localMin) / 2);
		
		center.push_back(localCenter);
	}
	
	return center;
}

double Hyperblock::getRatio(int numOfCoordinates) {
	double numOfParameters = numOfCoordinates + 1.0;
	return (numOfParameters * 2) / (this->pointsInBlock.size() * numOfParameters);
}

int Hyperblock::getSize() const {
	return this->size;
}