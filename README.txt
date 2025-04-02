This is a library to model, manipulate, and draw parallel coordinates.
It has three main components which are the ParallelCoordinates.h, PCAlgorithms.h, and Drawing.h headers.

ParallelCoordinates.h
This includes all data strucutres needed to store and manipulate parallel coordinates.
It contains Coordinates.h which contains Node.h which contains Point.h.
This represents the coordinates of a parallel coordinate visualizaiton and the data of these coordinates.
It also contains Class.h and Hyperblock.h which both contain Point.h.
They represent subsets of points with one to represent classes of the dataset and the other to represent hyperblocks.
It also contains Color.H which is used as a way to encapsulate RGBA values of colors.
It also contains functions to read csv files in order to populate the data structures. 

PCAlgorithms.h
This includes algorithms that use ParallelCoordinates objects.

Drawing.h
This includes algorithms for drawing ParallelCoordinates in different ways.

Example code to get started... 
Assuming an OpenGL context is available to be drawn on...

#include "ParallelCoordinates.h"
#include "PCAlgorithms.h"
#include "Drawing.h"

// Initialize data structures
ParallelCoordinates pc = ParallelCoordinates();

// Populate data structures with data
pc.readFile(filePath);

// Perform machine learning
autoHyperblock(&pc);

// Draw coordinates
drawCoordinates(&pc);

// Draw the hyperblocks
for (int i = 0; i < pc.getBlockAmount(); i++)
{
    drawBlock(&pc, worldWidth, worldHeight, i);
}