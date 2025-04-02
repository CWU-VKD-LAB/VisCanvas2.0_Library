/**
 * A class that wraps each data point in a coordinate.
 */

#ifndef Node_H
#define Node_H
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


// a sinmple class to hold and manage a piece of data
class Node {
public:
	// create a node with data of 0.0
	Node();
	// create a node with data of the passed value(data)
	Node(double data);
	// delete the object
	~Node();

	// gets the data held by the node
	double getData() const;
	// gets the data that the node was originally set to hold
	double getOriginalData() const;
	// sets the data the node holds
	void setData(double newData);
	// sets the data to the original
	void resetData();
	// multiplies the data held by the node, by the passed value(multiplier)
	void multiplyData(double multiplier);
	// divides the data held by the node, by the passed value(divisor)
	void divideData(double divisor);
	// add the passed value(addend) to the data held by the node
	void addToData(double addend);

private:
	// the field holding the original value of the node's data
	double originalData;
	// the field holding the current value of the data
	double currentData;

};

#endif