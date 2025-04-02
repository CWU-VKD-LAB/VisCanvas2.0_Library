/**
 * Encapsulates all data structures and models parallel coordinates.
 * Includes functions to create/manipulate classes, colors, coordinates, points, and hyperblocks.
 */

#ifndef ParallelCoordinates_H
#define ParallelCoordinates_H
#include "Node.h"
#include "Coordinate.h"
#include "Color.h"
#include "Class.h"
#include "Point.h"
#include "Hyperblock.h"

#include <vector>
#include <list>
#include <set>
#include <map>
#include <string> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


// a class to hold several points of data and manage interactions with the data
class ParallelCoordinates {
public:
	// create a blank class without any data
	ParallelCoordinates();
	// delete the object
	~ParallelCoordinates();

	// read data from the file at the passed path(filePath) and 
	bool readFile(std::string* filePath);
	// saves the data and all settings to the file at the passed location
	bool saveToFile(std::string * filePath);

	// sets whether point names will be read from basic files 
	bool setReadPointNames(bool newReadPointNames);
	// sets whether coordinate names will be read from basic files 
	bool setReadCoordinateNames(bool newReadPointNames);
	// sets whether class names will be read from basic files 
	bool setReadClassNames(bool newReadClassNames);

	// returns whether point names will be read from basic files 
	bool isReadPointNames();
	// returns whether coordinate names will be read from basic files 
	bool isReadCoordinateNames();
	// returns whether class names will be read from basic files 
	bool isReadClassNames();


	// get the amounts of class for the data
	int getClassAmount() const;
	// gets the total amount of points
	int getPointAmount() const;
	// gets the number of coordinates in the data
	int getCoordinateAmount() const;
	vector<Coordinate *>* getDataCoordinates();
	// inverts the coordinate
	void invertCoordinate(int coordinateIndex);
	// checks whether the coordinate is inverted
	bool isCoordinateInverted(int coordinateIndex);
	// gets the data in the point of the passed index(pointIndex), for the passed coordinate(indexOfData)
	double getData(int pointIndex, int indexOfData) const;
	// gets the original data in the point of the passed index(pointIndex), for the passed coordinate(indexOfData)
	double getOriginalData(int pointIndex, int indexOfData) const;
	// sets the data in the point of the passed index(pointIndex), for the passed coordinate(indexOfData), to the passed value(newDataValue)
	double setData(int setIndex, int indexOfData, double newDataValue);
	// moves the coordinate at the passed index(indexOfCoordinate) to the spot after the other index(indexBeforeInsertion)
	bool moveData(int indexOfCoordinate, int indexBeforeInsertion);



	// gets the name of the coordinate at the passed index(classIndex)
	std::string* getCoordinateName(int coordinateIndex);
	// sets the name of the coordinate at the passed index(coordinateIndex) to the passed string(newName)
	void setCoordinateName(int coordinateIndex, std::string* newName);

	// gets whether the calibration for the coordinate(coordinateIndex) is artificial or relative
	bool isArtificiallyCalibrated(int coordinateIndex);
	// sets the calibration to use the data's(not the artificial) maximum and minimum in coordinate at the passed index(coordinateIndex)
	void clearArtificialCalibration(int coordinateIndex);
	// sets the bounds to be used for artificial calibration at the passed index(coordinateIndex)
	void setCalibrationBounds(int coordinateIndex, double newMaximum, double newMinimum);
	// gets the artificial maximum for the coordinate at the passed index(coordinateIndex)
	double getArtificialMaximum(int coordinateIndex) const;
	// gets the artificial minimum for the coordinate at the passed index(coordinateIndex)
	double getArtificialMinimum(int coordinateIndex) const;

	// add the passed double(amountToAdd) to all the data in the coordinate at the index(coordinateIndex)
	void addToCoordinate(int coordinateIndex, double amountToAdd);




	// gets the name of the class at the passed index(classIndex)
	std::string* getClassName(int classIndex);
	// sets the name of the class at the passed index(classIndex) to the passed string(newName)
	void setClassName(int classIndex, std::string* newName);
	// adds a new class to the list of classes
	void addClass();
	// delete a class from the list of classes
	void deleteClass(int classIndex);
	/*
	Gets the number of points in the class at the passed index(classIndex)
	*/
	int getPointAmount(int classIndex) const;
	// gets a list of points in the class
	std::vector<int>* getPointsInClass(int classIndex);
	// gets the color for the class at the passed class index
	std::vector<double>* getClassColor(int classIndex);
	// sets the color for the class at the passed class index(classIndex) to the passed color(newColor)
	void setClassColor(int classIndex, std::vector<double>* newColor);



	vector<Hyperblock> getBlocks();
	// gets the name of the point in the class at the passed index(classIndex) at the passed class relative index(setIndex)
	std::string* getPointName(int pointIndex);
	// sets the name of the point at the passed index(pointIndex)
	void setPointName(int pointIndex, std::string &newName);
	// get the data class index for the point at the passed index
	int getClassOfPoint(int pointIndex) const;
	// get the data point name for the class at the passed index
	std::string* getPointOfClass(int classIndex, int pointIndex);
	// sets the index data class of the point at the passed index(pointIndex)
	int setPointClass(int pointIndex, int newClassIndex);
	// gets the color the point should be painted
	std::vector<double>* getPointColor(int pointIndex);
	// checks whether the set is visible or not
	bool isVisible(int pointIndex);
	// sets the set visible or not visible
	bool setVisible(int pointIndex, bool newVisible);



	// gets the amount the coordinate is shifted by
	double getCoordinateShift(int coordinateIndex);
	// changes the shift of the coordinate at the passed int to the passsed double
	void setCoordinateShift(int coordinateIndex, double shiftMod);



	// sorts the data by the point at the passed index, in ascending order(left to right)
	void sortAscending(int pointIndex);
	// sorts the data by the point at the passed index, in descending order(left to right)
	void sortDescending(int pointIndex);
	// places the coordinates back in the original order
	void sortOriginal();



	/* adjusts the data of each coordinate by shifting the data of each coordinate by the
	amount necessary to put the dat of the point at the passed index(pointIndex) to the passed value(levelValue)*/
	void level(int pointIndex, double levelValue);
	// gets the mean data value of the passed point
	double getMean(int pointIndex) const;
	// gets the median data value of the passed point
	double getMedian(int pointIndex) const;
	// calibrates each coordinate's data to the [0,1] space, coordinates calibrated individually
	void calibrateData();
	// zero shifts in the coordinates
	void zeroShifts();


	// compares the data of a each point to the point at the passed index and checks if the data is within the radius of the data of the passed point
	bool hyperblock(int pointIndex, double radius);
	// returns whether the blocks will use mean or median
	bool isUseMeanForBlocks();
	// sets whether the blocks will use mean or median
	bool getUseMeanForBlocks();
	// sets whether the blocks will use mean or median
	void setUseMeanForBlocks(bool newUseMean);
	double getRadius();
	void setRadius(double newRadius);

	// gets amount of blocks
	int getBlockAmount();
	// get blocks
	std::vector<Hyperblock> gettBlocks();
	// the minimum value for the block
	double getBlockMinimum(int blockIndex, int coordinateIndex) const;
	// the mean value for the block
	double getBlockMiddle(int blockIndex, int coordinateIndex) const;
	// the maximum value for the block
	double getBlockMaximum(int blockIndex, int coordinateIndex) const;
	// gets the color of the block
	std::vector<double>* getBlockColor(int blockIndex);
	// sets the color of the block at the passed index
	void setBlockColor(int blockIndex, std::vector<double>* newColor);
	// deletes the block at the passed index
	void deleteBlock(int classIndex);
	// gets the name of the block
	std::string* getBlockName(int blockIndex);
	// sets whether the named block is displayed
	void setDisplayed(string name, bool display);
	// gets whether a block is displayed
	bool getDisplayed(int index);
	// sets the name of the block
	void setBlockName(int blockIndex, std::string* newName);
	// gets a list of the points in the block
	std::vector<int>* getBlockPoints(int blockIndex);
	
	int getVisibleCoordinateCount();

	vector<Class> getClasses();
private:

	bool readPointNamesBasic;
	bool readCoordinateNamesBasic;
	bool readClassNamesBasic;


	// a vector to hold the coordinates containing the data for the points
	std::vector<Coordinate *> dataCoordinates;

	// a vector to hold the data class
	std::vector<Class> dataClasses;
	// a vector to hold the data points
	std::vector<Point> dataPoints;

	// holds the blocks of the data
	std::vector<Hyperblock> blocks;
	// holds the boolean of whether blocks use mean or median
	bool useMean;
	double radius;

	// a method to hold the basic intialization of the object fields
	void init();
	// a method to hold the setup of fields to be performed after everything else
	void finalInit();

	// reads the contents of the file, at fileName, into a vector
	std::vector<std::vector<std::string>*>* readFileIntoVector(std::string* fileName);

	// takes the passed string and tokenizes it by commas
	std::vector<std::string>* tokenizeString(std::string* stringToTokenize);

	// removes duplicate strings from the passed vector
	void removeDuplicates(std::vector<std::string>* stringList);

	// reads the contents of the file, at fileName, into a vector
	bool readBasicFile(std::vector<std::vector<std::string>*>* fileContents);

	// reads the contents of the file, at fileName, into a vector
	void readCustomFile(std::vector<std::vector<std::string>*>* fileContents);

	// parses a command line from a save file
	void parseLine(std::vector<std::string>* lineTokens);

	// counts the number of characters in the passed string(line) that are one of the characters in the passed vector(characters)
	int countCharacters(vector<char>* characters, string* line);



	// an ascending(left to right) merge sort of the passed coordinate list by the point at the passed index
	std::list<Coordinate*>* mergeSortAscending(std::list<Coordinate*>* listToSort, int pointIndex);

	// an descending(left to right) merge sort of the passed coordinate list by the point at the passed index
	std::list<Coordinate*>* mergeSortDescending(std::list<Coordinate*>* listToSort, int setIndex);

	// sorts the coordinates to their original places in a merge sort
	std::list<Coordinate*>* mergeSortOriginal(std::list<Coordinate*>* listToSort);

	
};

#endif