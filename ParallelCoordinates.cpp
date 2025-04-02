#include "ParallelCoordinates.h"
#include "Node.h"
#include "Coordinate.h"
#include "Color.h"
#include "Class.h"
#include "Point.h"
#include "Hyperblock.h"

#include <vector>
#include <set>
#include <list>
#include <string> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <thread>

using namespace std;

// create a blank class without any data
ParallelCoordinates::ParallelCoordinates() {
	readPointNamesBasic = true;
	readCoordinateNamesBasic = true;
	readClassNamesBasic = true;
	radius = 0.2;
	init();
	finalInit();
}


// delete the object
ParallelCoordinates::~ParallelCoordinates() {

	dataCoordinates.clear();
}


// parse the data in the file at the passed path into the object
bool ParallelCoordinates::readFile(string * filePath) {
	std::vector<std::vector<std::string>*>* fileLines = readFileIntoVector(filePath);
	if (fileLines == nullptr) {
		return false;
	}
	if (fileLines->size() < 2) {
		return false;
	}
	if ((*fileLines)[0]->size() <= 0) {
		return false;
	}
	// find if a custom file or a basic file
	// find first none blank line
	bool isCustomFileFormat = false;
	if (isCustomFileFormat = ((*(*fileLines)[0])[0] == "VisCanvas Save File Format")) {
		isCustomFileFormat = true;
	}

	for (unsigned int i = 0; i < dataCoordinates.size(); i++) {
		delete dataCoordinates[i];
	}
	dataCoordinates.clear();
	dataClasses.clear();
	dataPoints.clear();
	blocks.clear();
	init();

	try {
		if (isCustomFileFormat) {
			readCustomFile(fileLines);
		}
		else {
			readBasicFile(fileLines);
		}
	}
	catch (...) {
		dataCoordinates.clear();
		dataClasses.clear();
		dataPoints.clear();
		blocks.clear();
		init();
		finalInit();
		return false;
	}
	// perform final setup operations
	finalInit();
	return true;
}


// saves the data and all settings to the file at the passed location
bool ParallelCoordinates::saveToFile(std::string * filePath) {
	ofstream saveFile;
	saveFile.open(*filePath);
	// check if file is open
	if (saveFile.is_open() == false) {
		return false;
	}
	// write file type identifier
	saveFile << "VisCanvas Save File Format,";
	saveFile << ",";
	// write coordinate names
	for (int i = 0; i < getCoordinateAmount(); i++) {
		saveFile << *(dataCoordinates[i]->getName());
		if (i < getCoordinateAmount() - 1) {
			saveFile << ",";
		}
	}
	saveFile << "\n";
	// write data sets, their class and data
	for (int j = 0; j < getPointAmount(); j++) {
		// write set class and name
		std::string className = *(getClassName(getClassOfPoint(j)));
		std::string pointName = *(getPointName(j));
		saveFile << className << "," << pointName << ",";
		for (int i = 0; i < getCoordinateAmount(); i++) {
			double currentData = dataCoordinates[i]->getOriginalData(j);
			saveFile << currentData;
			if (i < getCoordinateAmount() - 1) {
				saveFile << ",";
			}
		}
		saveFile << "\n";
	}
	// put space between data and commands
	saveFile << "\n\n";

	// write point classes
	saveFile << "classes\n";
	for (int i = 1; i < getClassAmount(); i++) {
		std::vector<double>* color = getClassColor(i);
		saveFile << (*getClassName(i)) << ",";
		saveFile << (*color)[0] << "," << (*color)[1] << "," << (*color)[2] << "," << (*color)[3];
		saveFile << "\n";
	}
	saveFile << "\n\n";



	saveFile << "Commands\n";


	// print coordinate shifts
	for (int i = 0; i < getCoordinateAmount(); i++) {
		double shift = dataCoordinates[i]->getShift();
		// make sure shift is large enough to bother with
		if (abs(shift) > 0.001) {
			saveFile << "shift," << i << "," << shift << "\n";
		}
	}

	// print coordinate bounds
	for (int i = 0; i < getCoordinateAmount(); i++) {
		double min = dataCoordinates[i]->getArtificialMinimum();
		double max = dataCoordinates[i]->getArtificialMaximum();
		// make the artificial calibration exists
		if (dataCoordinates[i]->isArtificiallyCalibrated()) {
			saveFile << "bounds," << i << "," << max << "," << min << "\n";
		}
	}

	// print coordinate order
	saveFile << "original indexes,";
	for (int i = 0; i < getCoordinateAmount(); i++) {
		int originalIndex = this->dataCoordinates[i]->getOriginalIndex();
		saveFile << originalIndex;
		if (i < getCoordinateAmount() - 1) {
			saveFile << ",";
		}
	}
	saveFile << "\n";

	for (int i = 0; i < getBlockAmount(); i++) {
		saveFile << "hyperblock,";
		saveFile << blocks[i].getOriginalPoint() << ",";
		saveFile << blocks[i].getRadius() << "\n";
	}

	// print whether to use mean or median for blocks
	saveFile << "useMean,";
	saveFile << useMean << "\n";

	// put artificial calibration in the list of 'commands'
	for (int i = 0; i < getCoordinateAmount(); i++) {
		if(isArtificiallyCalibrated(i)){
			saveFile << "artificial calibration,";
			saveFile << i << ",";
			saveFile << dataCoordinates[i]->isArtificiallyCalibrated() << ",";
			saveFile << dataCoordinates[i]->getArtificialMaximum() << ",";
			saveFile << dataCoordinates[i]->getArtificialMinimum() << "\n";
		}
	}

	// put default class color in
	std::vector<double>* defaultClassColor = getClassColor(0);
	saveFile << "Default Class Color,";
	saveFile << (*defaultClassColor)[0] << ",";
	saveFile << (*defaultClassColor)[1] << ",";
	saveFile << (*defaultClassColor)[2] << ",";
	saveFile << (*defaultClassColor)[3] << "\n";


	// put inverted coordinates in
	for (int i = 0; i < getCoordinateAmount(); i++) {
		if (isCoordinateInverted(i)) {
			saveFile << "Invert Dimension," << i << "\n";
		}
	}


	// end the file
	saveFile << "\n";
	saveFile.close();
}

// sets whether point names will be read from basic files 
bool ParallelCoordinates::setReadPointNames(bool newReadPointNames) {
	bool previousVal = readPointNamesBasic;
	readPointNamesBasic = newReadPointNames;
	return previousVal;
}

// sets whether coordinate names will be read from basic files 
bool ParallelCoordinates::setReadCoordinateNames(bool newReadCoordinateNames) {
	bool previousVal = readCoordinateNamesBasic;
	readCoordinateNamesBasic = newReadCoordinateNames;
	return previousVal;
}

// sets whether class names will be read from basic files 
bool ParallelCoordinates::setReadClassNames(bool newReadClassNames) {
	bool previousVal = readCoordinateNamesBasic;
	readClassNamesBasic = newReadClassNames;
	return previousVal;
}

// returns whether point names will be read from basic files 
bool ParallelCoordinates::isReadPointNames() {
	return readPointNamesBasic;
}

// returns whether coordinate names will be read from basic files 
bool ParallelCoordinates::isReadCoordinateNames() {
	return readCoordinateNamesBasic;
}

// returns whether class names will be read from basic files 
bool ParallelCoordinates::isReadClassNames() {
	return readClassNamesBasic;
}


// gets the number of classes in the data
int ParallelCoordinates::getClassAmount() const {
	return dataClasses.size();
}

/*
Gets the total number of points in the data
full implementation after file parsing
*/
int ParallelCoordinates::getPointAmount() const {
	if (dataCoordinates.size() == 0) {
		return 0;
	}
	return (*dataCoordinates[0]).size();
}

// gets the number of coordinates in the data
int  ParallelCoordinates::getCoordinateAmount() const {
	return dataCoordinates.size();
}


// inverts the coordinate
void ParallelCoordinates::invertCoordinate(int coordinateIndex) {
	if (coordinateIndex >= this->getCoordinateAmount() || coordinateIndex < 0) {
		return;
	}

	(dataCoordinates[coordinateIndex])->invert();
}

// checks whether the coordinate is inverted
bool ParallelCoordinates::isCoordinateInverted(int coordinateIndex) {
	if (coordinateIndex >= this->getCoordinateAmount() || coordinateIndex < 0) {
		return false;
	}

	return (dataCoordinates[coordinateIndex])->isInverted();
}





// gets the data in the passed point at the passed index
double ParallelCoordinates::getData(int pointIndex, int indexOfData) const {
	// check if indexes are in bounds
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return 0.0;
	}
	if (indexOfData >= getCoordinateAmount() || indexOfData < 0) {
		return 0.0;
	}

	// get data
	return (*dataCoordinates[indexOfData]).getData(pointIndex);
}

// gets the original data in the point of the passed index(pointIndex), for the passed coordinate(indexOfData)
double ParallelCoordinates::getOriginalData(int pointIndex, int coordinateIndex) const {
	// check if indexes are in bounds
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return 0.0;
	}
	if (coordinateIndex >= getCoordinateAmount() || coordinateIndex < 0) {
		return 0.0;
	}

	// get data
	return (*dataCoordinates[coordinateIndex]).getOriginalData(pointIndex);
}


// sets the data in the point of the passed index(pointIndex), for the passed coordinate(indexOfData), to the passed value(newDataValue)
double ParallelCoordinates::setData(int pointIndex, int indexOfData, double newDataValue) {
	// check if indexes are in bounds
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return 0.0;
	}
	if (indexOfData >= getCoordinateAmount() || indexOfData < 0) {
		return 0.0;
	}

	// get data
	double oldData = (*dataCoordinates[indexOfData]).getOriginalData(pointIndex);
	(*dataCoordinates[indexOfData]).setData(pointIndex, newDataValue);
	(*dataCoordinates[indexOfData]).calibrateData();
	return oldData;
}

/*
moves the position of a coordinates from the passed index(indexOfCoordinate) to at the other passed index(indexOfInsertion)
Note: the new position is calculated before the coordinate being moved is removed from its place so if the data had coordinates 0,1,2,3
and the call moveData(1, 3) was made the coordinates would become 0,2,1,3 not 0,2,3,1
*/
bool ParallelCoordinates::moveData(int indexOfCoordinate, int indexOfInsertion) {
	if (indexOfInsertion == -1 || indexOfCoordinate == -1) {
		return false;
	}
	if (indexOfInsertion >= getCoordinateAmount() || indexOfCoordinate >= getCoordinateAmount()) {
		return false;
	}

	// this will swap the data 
	Coordinate *temp = dataCoordinates[indexOfCoordinate]; // temp = a
	dataCoordinates[indexOfCoordinate] = dataCoordinates[indexOfInsertion]; // a = b
	dataCoordinates[indexOfInsertion] = temp; // b = temp

	return true;
}




// gets the name of the class at the passed index(classIndex) to the passed string(newName)
std::string * ParallelCoordinates::getCoordinateName(int coordinateIndex) {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return nullptr;
	}
	return dataCoordinates[coordinateIndex]->getName();
}

// sets the name of the class at the passed index(classIndex)
void ParallelCoordinates::setCoordinateName(int coordinateIndex, string * newName) {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return;
	}
	dataCoordinates[coordinateIndex]->setName(newName);
}

// gets whether the calibration for the coordinate(coordinateIndex) is artificial or relative
bool ParallelCoordinates::isArtificiallyCalibrated(int coordinateIndex) {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return false;
	}
	return dataCoordinates[coordinateIndex]->isArtificiallyCalibrated();
}

// sets the calibration to use the data's(not the artificial) maximum and minimum in coordinate at the passed index(coordinateIndex)
void ParallelCoordinates::clearArtificialCalibration(int coordinateIndex) {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return;
	}
	dataCoordinates[coordinateIndex]->clearArtificialCalibration();
}

// sets the bounds to be used for artificial calibration at the passed index(coordinateIndex)
void ParallelCoordinates::setCalibrationBounds(int coordinateIndex, double newMaximum, double newMinimum) {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return;
	}

	// take care of this if the user puts the wrong input for min-max
	if (newMinimum > newMaximum) {
		// swap them
		double temp = newMinimum;
		newMinimum = newMaximum;
		newMaximum = temp;
	}

	dataCoordinates[coordinateIndex]->setCalibrationBounds(newMaximum, newMinimum);
}
// gets the artificial maximum for the coordinate at the passed index(coordinateIndex)
double ParallelCoordinates::getArtificialMaximum(int coordinateIndex) const {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return 0.0;
	}
	return dataCoordinates[coordinateIndex]->getArtificialMaximum();
}
// gets the artificial minimum for the coordinate at the passed index(coordinateIndex)
double ParallelCoordinates::getArtificialMinimum(int coordinateIndex) const {
	if (coordinateIndex >= dataCoordinates.size() || coordinateIndex < 0) {
		return 0.0;
	}
	return dataCoordinates[coordinateIndex]->getArtificialMinimum();
}


// add the passed double(amountToAdd) to all the data in the coordinate at the index(coordinateIndex)
void ParallelCoordinates::addToCoordinate(int coordinateIndex, double amountToAdd) {
	// don't accept out of bounds data set indexes
	if (coordinateIndex >= getCoordinateAmount() || coordinateIndex < 0) {
		return;
	}
	dataCoordinates[coordinateIndex]->addToData(amountToAdd);
}

vector<Coordinate*> * ParallelCoordinates::getDataCoordinates() {
	return &dataCoordinates;
}

// gets the name of the class at the passed index
// full implementation after file parsing
string* ParallelCoordinates::getClassName(int classIndex) {
	if (classIndex >= dataClasses.size() || classIndex < 0) {
		return nullptr;
	}
	return dataClasses[classIndex].getName();
}

// sets the name of the class at the passed index to the passed string and returns the old name
// full implementation after file parsing
void ParallelCoordinates::setClassName(int classIndex, string* newName) {
	if (dataClasses.size() <= classIndex || classIndex < 0) {
		return;
	}
	dataClasses[classIndex].setName(newName);
}

// adds a new class to the list of classes
void ParallelCoordinates::addClass() {
	dataClasses.push_back(Class(dataClasses.size(), "Class " + std::to_string(dataClasses.size())));
}

// delete a class from the list of classes
void ParallelCoordinates::deleteClass(int classIndex) {
	// do not allow deletion of default class
	if (classIndex <= 0 || classIndex >= getClassAmount()) {
		return;
	}

	// put the sets in the class to be deleted in the default class
	for (int i = 0; i < this->getPointAmount(); i++) {
		if (dataPoints[i].getClass() == classIndex) {
			dataPoints[i].setDataClass(0);
		}
	}

	// delete the class
	dataClasses.erase(dataClasses.begin() + classIndex);
}

/*
Gets the number of points in the class at the passed index(classIndex)
*/
int ParallelCoordinates::getPointAmount(int classIndex) const {
	if (classIndex < 0 || classIndex >= getClassAmount()) {
		return -1;
	}
	return dataClasses[classIndex].getPointNumber();
}

std::vector<int>* ParallelCoordinates::getPointsInClass(int classIndex) {
	// do not allow deletion of default class
	if (classIndex < 0 || classIndex >= getClassAmount()) {
		return nullptr;
	}
	return dataClasses[classIndex].getPointsInClass();
}

// sets the name of the class at the passed index(classIndex) to the passed string(newName)
std::vector<double>* ParallelCoordinates::getClassColor(int classIndex) {
	if (classIndex < 0 || classIndex >= dataClasses.size()) {
		return dataClasses[0].getColor();
	}
	return dataClasses[classIndex].getColor();
}

/*
Gets the number of sets in the class at the passed index(classIndex)
*/
void ParallelCoordinates::setClassColor(int classIndex, std::vector<double>* newColor) {
	if (classIndex < 0 || classIndex >= dataClasses.size()) {
		return;
	}
	if (newColor->size() < 4) {
		return;
	}

	dataClasses[classIndex].setColor(newColor);
}





// gets the name of the point at the pased index
// full implementation after file parsing
string* ParallelCoordinates::getPointName(int pointIndex) {
	// check for invalid indexes
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return nullptr;
	}
	// get the point name
	return dataPoints[pointIndex].getName();
}

// sets the name of the point at the passed index to the passed string and returns the old name
// full implementation after file parsing
void ParallelCoordinates::setPointName(int pointIndex, string  &newName) {
	// check for invalid indexes
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return;
	}
	// get the point name
	dataPoints[pointIndex].setName(newName);
}
/*
returns the index of the class containing the point at the passed index
returns -1 if the passed point index is out of bounds
*/
int ParallelCoordinates::getClassOfPoint(int pointIndex) const {
	// check if indexes are in bounds
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return -1;
	}
	return this->dataPoints[pointIndex].getClass();
}

// get the data point name for the class at the passed index
std::string *ParallelCoordinates::getPointOfClass(int classIndex, int pointIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0)
	{
		return nullptr;
	}

	if (classIndex >= this->getClassAmount() || classIndex < 0)
	{
		return nullptr;
	}
	std::string str = std::string("");
	if (this->dataPoints[pointIndex].getClass() == classIndex)
	{
		return this->dataPoints[pointIndex].getName();
	}
	else {
		return &str;
	}
}

// sets the index data class of the point at the passed index(pointIndex)
int ParallelCoordinates::setPointClass(int pointIndex, int newClassIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return -1;
	}
	if (newClassIndex >= this->getClassAmount() || newClassIndex < 0) {
		return -1;
	}
	dataClasses[dataPoints[pointIndex].getClass()].removePoint(pointIndex);
	dataClasses[newClassIndex].addPoint(pointIndex);
	return dataPoints[pointIndex].setDataClass(newClassIndex);
}

// gets the color the point should be painted
std::vector<double>* ParallelCoordinates::getPointColor(int pointIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return dataClasses[0].getColor();
	}

	std::vector<double>* color = dataClasses[dataPoints[pointIndex].getClass()].getColor();
	if (isVisible(pointIndex) == false) {
		(*color)[3] = 0.0;
	}
	return color;
}


// checks whether the point is visible or not
bool ParallelCoordinates::isVisible(int pointIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return false;
	}
	return dataPoints[pointIndex].isVisible();
}

// sets the point visible or not visible and returns the old visibility
bool ParallelCoordinates::setVisible(int pointIndex, bool newVisible) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return false;
	}
	return dataPoints[pointIndex].setVisible(newVisible);
}





// gets the amount the coordinate is shifted by
double ParallelCoordinates::getCoordinateShift(int coordinateIndex) {
	if (coordinateIndex >= this->getCoordinateAmount() || coordinateIndex < 0) {
		return 0.0;
	}
	return (*dataCoordinates[coordinateIndex]).getShift();


}

// changes the shift of the coordinate at the passed int to the passsed double
void ParallelCoordinates::setCoordinateShift(int coordinateIndex, double shiftMod) {
	if (coordinateIndex >= this->getCoordinateAmount() || coordinateIndex < 0) {
		return;
	}
	double currentShift = (*dataCoordinates[coordinateIndex]).getShift();
	return (*dataCoordinates[coordinateIndex]).shiftDataBy(shiftMod - currentShift);
}

// sorts the coordinates in ascending order by the data corresponding to the passed point index
void ParallelCoordinates::sortAscending(int pointIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return;
	}
	// construct list to sort
	std::list<Coordinate*> totalList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataCoordinates.size(); i++) {
		totalList.push_back(dataCoordinates[i]);
	}
	ptrTotalList = mergeSortAscending(ptrTotalList, pointIndex);

	// read sorted coordinates back into vector
	for (int i = 0; i < dataCoordinates.size(); i++) {
		dataCoordinates[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getBlockAmount(); i++) {
		blocks[i].calculateValues(&dataCoordinates);
	}
}

// sorts the coordinates in descending order by the data corresponding to the passed point index(pointIndex)
void ParallelCoordinates::sortDescending(int pointIndex) {
	if (pointIndex >= this->getPointAmount() || pointIndex < 0) {
		return;
	}
	// construct list to sort
	std::list<Coordinate*> totalList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataCoordinates.size(); i++) {
		totalList.push_back(dataCoordinates[i]);
	}
	ptrTotalList = mergeSortDescending(ptrTotalList, pointIndex);

	// read sorted coordinatess back into vector
	for (int i = 0; i < dataCoordinates.size(); i++) {
		dataCoordinates[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getBlockAmount(); i++) {
		blocks[i].calculateValues(&dataCoordinates);
	}
}

// places the coordinates back in the original order
void ParallelCoordinates::sortOriginal() {
	// construct list to sort
	std::list<Coordinate*> totalList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataCoordinates.size(); i++) {
		totalList.push_back(dataCoordinates[i]);
	}
	ptrTotalList = mergeSortOriginal(ptrTotalList);

	// read sorted coordinatess back into vector
	for (int i = 0; i < dataCoordinates.size(); i++) {
		dataCoordinates[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getBlockAmount(); i++) {
		blocks[i].calculateValues(&dataCoordinates);
	}
}


// shifts all the data of the passed point to the passed value
void ParallelCoordinates::level(int pointIndex, double levelValue) {
	// don't accept out of bounds data set indexes
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return;
	}
	zeroShifts();
	// adjust each data point to the new 0 amount for its coordinate
	for (unsigned int i = 0; i < getCoordinateAmount(); i++) {
		double currentData = (*dataCoordinates[i]).getCalibratedData(pointIndex);
		double change = levelValue - currentData;
		// change data
		(*dataCoordinates[i]).shiftDataBy(change);
	}
}

// gets the mean data value of the set at the passed index
double ParallelCoordinates::getMean(int pointIndex) const {
	// don't accept out of bounds data point indexes
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return 0.0;
	}
	double sum = 0.0;
	// adjust each data point to the new 0 amount for its coordinate
	for (unsigned int i = 0; i < getCoordinateAmount(); i++) {
		double currentData = (*dataCoordinates[i]).getCalibratedData(pointIndex);
		sum += currentData;
	}
	return sum /= ((double)getCoordinateAmount());
}

// gets the median data value of the point at the passed index
double ParallelCoordinates::getMedian(int pointIndex) const {
	// don't accept out of bounds data point indexes
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return 0.0;
	}
	if (getCoordinateAmount() == 0) {
		return 0.0;
	}
	// make a vector to hold the point data
	vector<double> pointData = vector<double>();
	// adjust each data point to the new 0 amount for its coordinate
	for (unsigned int i = 0; i < getCoordinateAmount(); i++) {
		double currentData = (*dataCoordinates[i]).getCalibratedData(pointIndex);
		pointData.push_back(currentData);
	}
	stable_sort(pointData.begin(), pointData.end());
	// acount for an even number of coordinates
	if (pointData.size() % 2 == 0) {
		double first = pointData[pointData.size() / 2];
		double second = pointData[pointData.size() / 2 - 1];
		return (first + second) / 2.0;
	}
	return pointData[pointData.size() / 2];

}

// calibrate each coordinate to the [0,1] space
void ParallelCoordinates::calibrateData() {
	for (unsigned int i = 0; i < getCoordinateAmount(); i++) {
		(*dataCoordinates[i]).calibrateData();
	}
}

// zero shifts in the coordinates
void ParallelCoordinates::zeroShifts() {
	for (unsigned int i = 0; i < getCoordinateAmount(); i++) {
		double shift = (*dataCoordinates[i]).getShift();
		(*dataCoordinates[i]).shiftDataBy(-shift);
	}
}


// compares the data of a each point to the point at the passed index and checks if the data is within the radius of the data of the passed point
// if the passed bool(meanMedian) is true the mean will be used and otherwise the median will be
bool ParallelCoordinates::hyperblock(int pointIndex, double radius) {

	
	// don't accept out of bounds data point indexes
	if (pointIndex >= getPointAmount() || pointIndex < 0) {
		return false;
	}
	if (radius < 0) {
		return false;
	}
	for (int i = 0; i < dataCoordinates[0]->size(); i++) {
		int clusterSets = 0;
	}

	std::string blockName = *(dataPoints[pointIndex].getName()) + " " + std::to_string(radius);

	for (int i = 0; i < getBlockAmount(); i++)
	{
		if (getBlocks().at(i).getName()->compare(blockName) == 0)
		{
			return false;
		}
	}

	std::vector<int> selectedPoints = std::vector<int>();

	for (int i = 0; i < getPointAmount(); i++)
	{
		bool withinCube = true;
		for (int j = 0; j < getCoordinateAmount(); j++)
		{
			if (!(getData(i, j) >= getData(pointIndex, j) - radius && getData(i, j) <= getData(pointIndex, j) + radius))
			{
				withinCube = false;
			}
		}
		if (withinCube)
		{
			selectedPoints.push_back(i);
		}
	}

	Color blockColor = Color();
	int originalSetClass = getClassOfPoint(pointIndex);
	std::vector<double>* colorConponents = getClassColor(originalSetClass);
	blockColor.setRed((*colorConponents)[0]);
	blockColor.setGreen((*colorConponents)[1]);
	blockColor.setBlue((*colorConponents)[2]);
	blockColor.setAlpha((*colorConponents)[3]);
	blocks.push_back(Hyperblock(blockColor, &selectedPoints/*, &dataCoordinates*/));
	blocks[blocks.size() - 1].setRadius(radius);
	blocks[blocks.size() - 1].setOriginalPoint(pointIndex);
	blocks[blocks.size() - 1].setName(&blockName);
	if (useMean) {
		blocks[blocks.size() - 1].setUseMean(useMean);
		blocks[blocks.size() - 1].calculateValues(&dataCoordinates);
	}
	

	return true;
}

// returns whether the blocks will use mean or median
bool ParallelCoordinates::isUseMeanForBlocks() {
	return useMean;
}

// returns whether the blocks will use mean or median
bool ParallelCoordinates::getUseMeanForBlocks() {
	return useMean;
}

// returns whether the blocks will use mean or median
void ParallelCoordinates::setUseMeanForBlocks(bool newUseMean) {
	useMean = newUseMean;
	for (int i = 0; i < getBlockAmount(); i++) {
		blocks[i].setUseMean(newUseMean);
	}

}

double ParallelCoordinates::getRadius() {
	return radius;
}

void ParallelCoordinates::setRadius(double newRadius) {
	radius = newRadius;
}

// gets amount of blocks
int ParallelCoordinates::getBlockAmount() {
	return blocks.size();
}

// get set blocks
std::vector<Hyperblock> ParallelCoordinates::getBlocks() {
	return blocks;
}

// get displayed of block at index
bool ParallelCoordinates::getDisplayed(int index) {
	return blocks[index].isDisplayed();
}

// the minimum value for the blocks data
double ParallelCoordinates::getBlockMinimum(int blockIndex, int coordinateIndex) const {
	if (coordinateIndex >= getCoordinateAmount() || coordinateIndex < 0) {
		return 0.0;
	}
	return blocks[blockIndex].getMinimum(coordinateIndex) + dataCoordinates[coordinateIndex]->getShift();
}

// the mean value for the blocks data
double ParallelCoordinates::getBlockMiddle(int blockIndex, int coordinateIndex) const {
	if (coordinateIndex >= getCoordinateAmount() || coordinateIndex < 0) {
		return 0.0;
	}
	return blocks[blockIndex].getMiddle(coordinateIndex) + dataCoordinates[coordinateIndex]->getShift();
}

// the maximum value for the blocks data
double ParallelCoordinates::getBlockMaximum(int blockIndex, int coordinateIndex) const {
	if (coordinateIndex >= getCoordinateAmount() || coordinateIndex < 0) {
		return 0.0;
	}
	return blocks[blockIndex].getMaximum(coordinateIndex) + dataCoordinates[coordinateIndex]->getShift();
}

// gets the color of the block
std::vector<double>* ParallelCoordinates::getBlockColor(int blockIndex) {
	if (blockIndex <0 || blockIndex>getBlockAmount()) {
		return nullptr;
	}
	return blocks[blockIndex].getColor();
}

// sets the color of the block at the passed index
void ParallelCoordinates::setBlockColor(int blockIndex, std::vector<double>* newColor) {
	if (blockIndex < 0 || blockIndex >= getBlockAmount()) {
		return;
	}
	if (newColor->size() < 4) {
		return;
	}
	blocks[blockIndex].setColor(newColor);
}

// delete a class from the list of classes
void ParallelCoordinates::deleteBlock(int blockIndex) {
	
	// do not allow deletion of default class
	if (blockIndex < 0 || blockIndex >= getBlockAmount()) {
		return;
	}
	// delete the class
	blocks.erase(blocks.begin() + blockIndex);
}

// gets the name of the block
std::string * ParallelCoordinates::getBlockName(int blockIndex) {
	if (blockIndex < 0 || blockIndex >= getBlockAmount()) {
		return nullptr;
	}
	return blocks[blockIndex].getName();
}

// sets the name of the block
void ParallelCoordinates::setBlockName(int blockIndex, std::string* newName) {
	if (blockIndex < 0 || blockIndex >= getBlockAmount()) {
		return;
	}
	return blocks[blockIndex].setName(newName);
}

// gets a list of the points in the class
std::vector<int>* ParallelCoordinates::getBlockPoints(int blockIndex) {
	if (blockIndex < 0 || blockIndex >= getBlockAmount()) {
		return nullptr;
	}
	return blocks[blockIndex].getPoints();
}




// private:
// a method to hold the basic intialization of the object fields
void ParallelCoordinates::init() {

	dataCoordinates = std::vector<Coordinate*>();
	// create and set the fields
	dataClasses = std::vector<Class>();
	dataClasses.push_back(Class(0, "Default"));
	std::vector<double> newColor = std::vector<double>();
	newColor.push_back(0.0);
	newColor.push_back(0.0);
	newColor.push_back(1.0);
	newColor.push_back(1.0);
	dataClasses[0].setColor(newColor);
	dataPoints = std::vector<Point>();
	blocks = std::vector<Hyperblock>();

	useMean = false;
}

// a method to hold the setup of fields to be performed after everything else
void ParallelCoordinates::finalInit() {

}

std::vector<std::vector<std::string>*>* ParallelCoordinates::readFileIntoVector(std::string* fileName) {
	std::ifstream file(*fileName);
	std::string str;
	std::stringstream ss;
	std::vector<std::vector<std::string>*>* fileLines = new std::vector<std::vector<std::string>*>();
	// read data
	while (std::getline(file, str))
	{
		// make new data point
		fileLines->push_back(tokenizeString(&str));
	}
	ss.clear();
	file.close();
	// validate file
	// check that file had at least one none empty line
	if (fileLines->size() <= 0) {
		return nullptr;
	}

	return fileLines;
}

// takes the passed string and tokenizes it by commas
std::vector<std::string>* ParallelCoordinates::tokenizeString(std::string * stringToTokenize) {
	std::vector<std::string>* stringTokens = new std::vector<std::string>();
	if (stringToTokenize->size() <= 0) {
		return stringTokens;
	}
	int startToken = 0;
	int endToken = stringToTokenize->find_first_of(',', startToken);
	int lengthToken = endToken - startToken;
	if (endToken == std::string::npos) {
		endToken = stringToTokenize->size();
	}
	// parse string
	while (startToken < stringToTokenize->size()) {
		// parse token
		std::string currentToken = (stringToTokenize->substr(startToken, lengthToken));
		// add the new token to the next
		stringTokens->push_back(currentToken);
		// find next token
		startToken = endToken + 1;
		endToken = stringToTokenize->find_first_of(',', startToken);
		if (endToken == std::string::npos) {
			endToken = stringToTokenize->size();
		}
		lengthToken = endToken - startToken;
	}
	return stringTokens;
}

// removes duplicate strings from the passed vector
void ParallelCoordinates::removeDuplicates(std::vector<std::string>* stringList) {
	std::vector<std::string> uniqueList = std::vector<std::string>();
	for (int i = 0; i < stringList->size(); i++) {
		std::string newClassName = (*stringList)[i];
		// check if the class already exists
		bool isNewClass = true;
		for (int j = 0; j < uniqueList.size(); j++) {
			if (uniqueList[j] == newClassName) {
				isNewClass = false;
				j = uniqueList.size();
			}
		}
		if (isNewClass) {
			uniqueList.push_back(newClassName);
		}
	}
	stringList->clear();

	for (int i = 0; i < uniqueList.size(); i++) {
		stringList->push_back(uniqueList[i]);
	}
}

// reads the contents of the file, at fileName, into a vector
bool ParallelCoordinates::readBasicFile(std::vector<std::vector<std::string>*>* fileContents) {
	
	try {

		// get number of points
		int setNumber = fileContents->size();
		int startRow = 0;
		int startColumn = 0;
		int endColumn = (*fileContents)[0]->size();
		int off = 0;
		int dimOff;

		// create data classes
		// create default class
		dataClasses.clear();
		dataClasses.push_back(Class(0, "Default"));
		std::vector<double> newColor = std::vector<double>();
		newColor.push_back(0.0);
		newColor.push_back(0.0);
		newColor.push_back(1.0);
		newColor.push_back(1.0);
		dataClasses[0].setColor(newColor);

		if (readPointNamesBasic)
		{
			off = 1;
			startColumn = 1; // actual data starts here
		}

		std::map<std::string, int> classIdx;
		if (readClassNamesBasic)
		{
			endColumn--; // actual data ends here

			std::set<std::string> classes;
			for (int i = startRow; i < fileContents->size(); i++)
			{
				classes.insert((*fileContents)[i]->back());
			}

			for (auto x : classes)
			{
				addClass();
				dataClasses[getClassAmount() - 1].setName(&x);
				classIdx[x] = getClassAmount() - 1;
				cout << x << endl;
			}
		}

		if (readCoordinateNamesBasic) 
		{
			dimOff = 1;
			setNumber--; // first row is not data
			startRow = 1; // data starts here
		}

		// create coordinates
		for (int i = startColumn; i < endColumn; i++)
		{

			dataCoordinates.push_back(new Coordinate(i - off, setNumber));
			dataCoordinates[i - off]->setName(&(*(*fileContents)[0])[i]);

			// populate coordinates with data
			for (int j = startRow; j < fileContents->size(); j++)
			{
				double newData = std::stod((*(*fileContents)[j])[i]);

				dataCoordinates[i - off]->setData(j - off, newData);
			}

			 dataCoordinates[i - off]->calibrateData();
		}

		// Give each point a name 
		for (int i = startRow; i < fileContents->size(); i++)
		{
			std::string newSetName;
			if (readPointNamesBasic)
			{
				newSetName = (*(*fileContents)[i])[0];
			}
			else
			{
				newSetName = std::to_string(i - dimOff);
			}

			dataPoints.push_back(Point(i - dimOff, classIdx[std::string((*fileContents)[i]->back())]));
			dataPoints[i - dimOff].setName(newSetName);
			dataClasses[classIdx[std::string((*fileContents)[i]->back())]].addPoint(i - dimOff);
		}

	}
	catch (...) {
		return false;
	}

	return true;
}

vector<Class> ParallelCoordinates::getClasses() {
	return this->dataClasses;
}

// reads the contents of the file, at fileName, into a vector
void ParallelCoordinates::readCustomFile(std::vector<std::vector<std::string>*>* fileContents) {
	// ensure file has data
	if (fileContents->size() < 2) {
		return;
	}
	int lastDataLine = 0;
	// find the last line of data
	int tokenNumber = (*fileContents)[0]->size();
	for (int i = 1; i < fileContents->size(); i++) {
		if (tokenNumber != (*fileContents)[i]->size()) {
			i = fileContents->size();
		}
		else {
			lastDataLine = i;
		}
	}
	if (lastDataLine < 1) {
		return;
	}
	// create the data classes
	// find the start of the class section
	int classSectionFirstLine = lastDataLine + 1;
	for (int i = classSectionFirstLine; i < fileContents->size(); i++) {
		if ((*(*fileContents)[i]).size() > 0) {
			if ((*(*fileContents)[i])[0].compare("classes") == 0) {
				classSectionFirstLine = i + 1;
				i = fileContents->size();
			}
		}
	}
	// find the line after the end of the class section
	int classSectionAfterLastLine = classSectionFirstLine;
	for (int i = classSectionFirstLine; i < fileContents->size(); i++) {
		if ((*(*fileContents)[i]).size() <= 0) {
			classSectionAfterLastLine = i;
			i = fileContents->size();
		}
		else if ((*(*fileContents)[i])[0].compare("") == 0) {
			break;
		}
	}
	dataClasses.clear();
	dataClasses.push_back(Class(0, "Default"));
	std::vector<double> newColor = std::vector<double>();
	newColor.push_back(0.0);
	newColor.push_back(0.0);
	newColor.push_back(1.0);
	newColor.push_back(1.0);
	dataClasses[0].setColor(newColor);
	// check if there are classes to create
	if (classSectionFirstLine < classSectionAfterLastLine) {
		// create the classes
		for (int i = classSectionFirstLine; i < classSectionAfterLastLine; i++) {
			std::string className = (*(*fileContents)[i])[0];
			std::vector<double> colorComponents = std::vector<double>();
			colorComponents.push_back(stod((*(*fileContents)[i])[1]));
			colorComponents.push_back(stod((*(*fileContents)[i])[2]));
			colorComponents.push_back(stod((*(*fileContents)[i])[3]));
			colorComponents.push_back(stod((*(*fileContents)[i])[4]));
			this->addClass();
			dataClasses[getClassAmount() - 1].setName(&className);
			dataClasses[getClassAmount() - 1].setColor(colorComponents);
		}

	}

	// get number of sets
	int setNumber = lastDataLine;
	// get coordinate names
	std::vector<std::string> headers = std::vector<std::string>((*fileContents)[0]->size() - 2);
	for (int i = 2; i < (*fileContents)[0]->size(); i++) {
		std::string newCoordinateName = (*(*fileContents)[0])[i];
		dataCoordinates.push_back(new Coordinate(i - 2, setNumber));
		dataCoordinates[i - 2]->setName(&newCoordinateName);
	}

	// create data sets
	for (int i = 1; i <= lastDataLine; i++) {
		std::string newPointName = (*(*fileContents)[i])[1];
		std::string setClassName = (*(*fileContents)[i])[0];
		int classIndex = 0;
		for (int j = 0; j < dataClasses.size(); j++) {
			if (dataClasses[j].getName()->compare(setClassName) == 0) {
				classIndex = j;
				j = dataClasses.size();
			}
		}
		dataPoints.push_back(Point(i - 1, classIndex));
		dataPoints[dataPoints.size()-1].setName(newPointName);
		dataClasses[classIndex].addPoint(i - 1);
	}

	// add data structure
	for (int i = 2; i < (*fileContents)[0]->size(); i++) {
		Coordinate* currentCoordinate = dataCoordinates[i - 2];
		for (int j = 1; j <= lastDataLine; j++) {
			double newData = std::stod((*(*fileContents)[j])[i]);
			currentCoordinate->setData(j - 1, newData);
		}
		currentCoordinate->calibrateData();
	}

	for (int i = classSectionAfterLastLine + 1; i < fileContents->size(); i++) {
		parseLine((*fileContents)[i]);
	}
}
// parses a command line from a save file
void ParallelCoordinates::parseLine(std::vector<std::string>* lineTokens) {
	if (lineTokens == nullptr) {
		// do nothing
	}
	else if (lineTokens->size() == 0) {
		// do nothing
	}
	// read data
	else if ((*lineTokens)[0].compare("shift") == 0) {
		if (lineTokens->size() >= 3) {
			int index = stoi((*lineTokens)[1]);
			double shift = stod((*lineTokens)[2]);
			if (index < 0 || index >= this->getPointAmount()) {
				return;
			}
			else {
				this->dataCoordinates[index]->shiftDataBy(shift);
			}
		}
	}
	else if ((*lineTokens)[0].compare("bounds") == 0) {
		if (lineTokens->size() >= 4) {
			int dimIndex = stoi((*lineTokens)[1]);
			double max = stod((*lineTokens)[2]);
			double min = stod((*lineTokens)[3]);
			if (dimIndex < 0 || dimIndex >= getCoordinateAmount()) {
				return;
			}
			dataCoordinates[dimIndex]->setCalibrationBounds(max, min);
		}
	}
	else if ((*lineTokens)[0].compare("original indexes") == 0) {
		if (lineTokens->size() >= this->getCoordinateAmount() + 1) {
			for (int i = 0; i < getCoordinateAmount(); i++) {
				int originalIndex = stoi((*lineTokens)[i + 1]);
				this->dataCoordinates[i]->setOriginalIndex(originalIndex);
			}
		}
	}
	else if ((*lineTokens)[0].compare("hypercube") == 0) {
		if (lineTokens->size() >= 3) {
			int index = stoi((*lineTokens)[1]);
			double radius = stod((*lineTokens)[2]);
			if (index < 0 || index >= this->getPointAmount()) {
				return;
			}
			else {
				if (radius < 0) {
					radius *= -1;
				}
				this->hyperblock(index, radius);
			}
		}
	}
	else if ((*lineTokens)[0].compare("useMean") == 0) {
		if (lineTokens->size() >= 2) {
			bool newUseMean = stoi((*lineTokens)[1]);
			this->setUseMeanForBlocks(newUseMean);
		}
	}
	else if ((*lineTokens)[0].compare("artificial calibration") == 0) {
		if (lineTokens->size() >= 5) {
			int index = stoi((*lineTokens)[1]);
			bool isCalibrated = stoi((*lineTokens)[2]);
			double maximum = stod((*lineTokens)[3]);
			double minimum = stod((*lineTokens)[4]);
			if (index < 0 || index >= this->getCoordinateAmount()) {
				return;
			}
			this->setCalibrationBounds(index, maximum, minimum);
			if (isCalibrated == false) {
				this->dataCoordinates[index]->clearArtificialCalibration();
			}
		}
	}
	else if ((*lineTokens)[0].compare("Default Class Color") == 0) {
		if (lineTokens->size() >= 5) {
			double red = stod((*lineTokens)[1]);
			double green = stod((*lineTokens)[2]);
			double blue = stod((*lineTokens)[3]);
			double alpha = stod((*lineTokens)[4]);
			std::vector<double> color = std::vector<double>();
			color.push_back(red);
			color.push_back(green);
			color.push_back(blue);
			color.push_back(alpha);
			dataClasses[0].setColor(&color);
		}
	}
	else if ((*lineTokens)[0].compare("Invert Dimension") == 0) {
		if (lineTokens->size() >= 2) {
			int coordinateIndex = stoi((*lineTokens)[1]);
			invertCoordinate(coordinateIndex);
		}
	}
	
}


// count number of characters
int ParallelCoordinates::countCharacters(vector<char>* characters, string* line)
{
	int count = 0;
	// iterate through all characters in passed linep
	for (int i = 0; i < line->size(); i++) {
		// check the character against each character in the passed vector
		for (int j = 0; j < characters->size(); j++) {
			if (line->at(i) == (*characters)[j]) {
				count++;
				j = (*characters).size();
			}
		}
	}
	return count;
}





// an ascending(left to right) merge sort of the passed coordinate list by the set at the passed index
std::list<Coordinate*>* ParallelCoordinates::mergeSortAscending(std::list<Coordinate*>* listToSort, int pointIndex) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}
	std::list<Coordinate*> leftList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrLeftList = &leftList;
	std::list<Coordinate*> rightList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Coordinate* currentCoordinate = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentCoordinate);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentCoordinate = listToSort->back();
			listToSort->pop_back();
			ptrRightList->push_front(currentCoordinate);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortAscending(ptrLeftList, pointIndex);
	ptrRightList = mergeSortAscending(ptrRightList, pointIndex);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		double dataLeft = ptrLeftList->front()->getData(pointIndex);
		double dataRight = ptrRightList->front()->getData(pointIndex);
		// find which to take
		if (dataLeft <= dataRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining coordinates
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

// an descending(left to right) merge sort of the passed coordinate list by the set at the passed index
std::list<Coordinate*>* ParallelCoordinates::mergeSortDescending(std::list<Coordinate*>* listToSort, int pointIndex) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}

	std::list<Coordinate*> leftList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrLeftList = &leftList;
	std::list<Coordinate*> rightList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Coordinate* currentCoordinate = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentCoordinate);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentCoordinate = listToSort->back();
			listToSort->pop_back();
			rightList.push_front(currentCoordinate);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortDescending(ptrLeftList, pointIndex);
	ptrRightList = mergeSortDescending(ptrRightList, pointIndex);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		double dataLeft = ptrLeftList->front()->getData(pointIndex);
		double dataRight = ptrRightList->front()->getData(pointIndex);
		// find which to take
		if (dataLeft >= dataRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining coordinates
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

// sorts the coordinates to their original places in a merge sort
std::list<Coordinate*>* ParallelCoordinates::mergeSortOriginal(std::list<Coordinate*>* listToSort) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}

	std::list<Coordinate*> leftList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrLeftList = &leftList;
	std::list<Coordinate*> rightList = std::list<Coordinate*>();
	std::list<Coordinate*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Coordinate* currentCoordinate = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentCoordinate);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentCoordinate = listToSort->back();
			listToSort->pop_back();
			rightList.push_front(currentCoordinate);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortOriginal(ptrLeftList);
	ptrRightList = mergeSortOriginal(ptrRightList);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		int indexLeft = ptrLeftList->front()->getOriginalIndex();
		int indexRight = ptrRightList->front()->getOriginalIndex();
		// find which to take
		if (indexLeft <= indexRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining coordinates
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

void ParallelCoordinates::setDisplayed(string name, bool displayed)
{
	for (int i = 0; i < blocks.size(); i++)
	{
		if (blocks[i].getName()->compare(name) == 0)
		{
			blocks[i].setDisplayed(displayed);
		}
	}
}

int ParallelCoordinates::getVisibleCoordinateCount() {
	int visibleCount = 0;
	for (int i = 0; i < getCoordinateAmount(); i++)
	{
		if (dataCoordinates[i]->isVisible()) visibleCount++;
	}
	return visibleCount;
}