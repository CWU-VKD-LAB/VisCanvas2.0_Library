#include "Node.h"

// create a node with data of 0.0
Node::Node() {
	currentData = 0.0;
	originalData = 0.0;
}

// create a node with data of the passed value(data)
Node::Node(double data) {
	currentData = data;
	originalData = data;
}

// delete the object
Node::~Node() {
}

// gets the data held by the node
double Node::getData() const {
	return currentData;
}

// gets the data that the node was originally set to hold
double Node::getOriginalData() const {
	return originalData;
}


// sets the data the node holds
void Node::setData(double newData) {
	currentData = newData;
	originalData = newData;
}

// sets the data to the original
void Node::resetData() {
	currentData = originalData;
}

// multiplies the data held by the node, by the passed value(multiplier)
void Node::multiplyData(double multiplier) {
	currentData *= multiplier;
}

// divides the data held by the node, by the passed value(divisor)
void Node::divideData(double divisor) {
	currentData /= divisor;
}

// add the passed value(addend) to the data held by the node
void Node::addToData(double addend) {
	currentData += addend;
}
