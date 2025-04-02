/**
 * A class of functions to draw parallel coordinates in different ways.
 */

#include "ParallelCoordinates.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

void drawBlock(ParallelCoordinates * pc, double worldWidth, double worldHeight, int blockIdx) {
    int coordinateCount = 0;
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);
    std::vector<double>* colorOfCurrent = pc->getBlockColor(blockIdx);
    glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
    glColor4d(192.0, 192.0, 192.0, 0.01);

    // draw block
    for (int k = 0; k < pc->getBlockPoints(blockIdx)->size(); k++)
    {
        if (pc->getDisplayed(blockIdx))
        {
            int currentIndex = pc->getBlockPoints(blockIdx)->at(k);

            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < pc->getCoordinateAmount(); i++)
            {
                
                std::vector<double>* colorOfCurrent = pc->getPointColor(currentIndex);
                glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
                
                if (pc->getDataCoordinates()->at(i)->isVisible())
                {
                    double currentData = pc->getData(currentIndex, i);
                    glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)), (currentData * (worldHeight * 0.5)) + (0.175 * worldHeight));

                    coordinateCount++;
                }
            }
            glEnd();
            coordinateCount = 0;
        }
    }
    glFlush();    
}

void drawOverlaps(ParallelCoordinates * pc, double worldWidth, double worldHeight) {
            
    vector<Hyperblock> blocks = pc->getBlocks();
    int coordinateCount = 0;
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);
    glLineWidth(3.0);
    glColor4d(0.0, 0.0, 0.0, 0.1);
    
    vector<vector<int>> pairs = vector<vector<int>>();
    
    // find the set of all pairs of blocks
    for (int i = 0; i < pc->getBlockAmount(); i++)
    {
        for (int j = 0; j < pc->getBlockAmount(); j++)
        {
            if (i == j) continue;
            if (pc->getClassOfPoint(pc->getBlocks()[i].getPoints()->at(0)) == pc->getClassOfPoint(pc->getBlocks()[j].getPoints()->at(0))) continue;

            bool alreadyExists = false;
            for (int k = 0; k < pairs.size(); k++)
            {
                if ((pairs[k][0] == i && pairs[k][1] == j) ||
                    (pairs[k][0] == j && pairs[k][1] == i))
                {
                    alreadyExists = true;
                }
            }
            
            if (alreadyExists) continue;
            
            vector<int> pair = { i , j };
            pairs.push_back(pair);
        }
    }

    glColor4d(0.0, 0.0, 0.0, 1.0);

    // draw all non-overlapping coordinatess for each pair
    for (int i = 0; i < pairs.size(); i++)
    {
        coordinateCount = 0;
        int firstIdx = pairs[i][0];
        int secondIdx = pairs[i][1];

        for (int j = 0; j < pc->getCoordinateAmount(); j++)
        {
            // if blocks overlapping, then draw rectangles
            if (blocks[firstIdx].getMaximum(j) <= blocks[secondIdx].getMinimum(j) ||
                blocks[firstIdx].getMinimum(j) >= blocks[secondIdx].getMaximum(j))
            {
                double floor = max(blocks[secondIdx].getMinimum(j), blocks[firstIdx].getMinimum(j));
                double ceil = min(blocks[secondIdx].getMaximum(j), blocks[firstIdx].getMaximum(j));

                ///// draw first rectangle /////
                glColor4d(0.0, 0.0, 0.0, (1.0 / 5));

                glBegin(GL_QUADS);
                
                // draw bottom left
                glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) - (xAxisIncrement / 4),
                    (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

                // draw top left
                glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) - (xAxisIncrement / 4),
                    (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

                // draw top right
                glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + (xAxisIncrement / 4),
                    (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

                // draw bottom right
                glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + (xAxisIncrement / 4),
                    (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));


                glEnd();

            }
            coordinateCount++;

        } // end coordinate loop

    } // end pairs loop
    glFlush();
}

void drawFrequencyAllData(ParallelCoordinates * pc, double worldWidth, double worldHeight) {
    vector<double> leftCoordinate = vector<double>();
    vector<double> rightCoordinate = vector<double>();
    vector<double> frequency = vector<double>();
    vector<int> colorIdx = vector<int>();
    bool alreadyExists = false;

    int coordinateCount = 0;
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);

    glColor4d(0.0, 0.0, 0.0, 1.0);

    // create array of coordinates and counts
    for (int i = 0; i < pc->getCoordinateAmount() - 1; i++)
    {
        if (!(pc->getDataCoordinates()->at(i)->isVisible())) continue;
        if (pc->getVisibleCoordinateCount() < 2) break;

        leftCoordinate.clear();
        rightCoordinate.clear();
        frequency.clear();
        colorIdx.clear();
        alreadyExists = false;

        int j = i + 1;
        while (j < pc->getCoordinateAmount() && !(pc->getDataCoordinates()->at(j)->isVisible())) j++;
        if (j >= pc->getCoordinateAmount()) continue;

        for (int k = 0; k < pc->getPointAmount(); k++)
        {
            double left = pc->getData(k, i);
            double right = pc->getData(k, j);

            // see if exists already, if so increment count; else add and count = 1
            alreadyExists = false;
            for (int l = 0; l < frequency.size(); l++)
            {
                if (leftCoordinate[l] == left &&
                    rightCoordinate[l] == right)
                {
                    alreadyExists = true;
                    frequency[l] = frequency[l] + 1.0;
                    break; // break out of l loop
                }
            } // end l loop

            if (!alreadyExists)
            {
                leftCoordinate.push_back(left);
                rightCoordinate.push_back(right);
                frequency.push_back(1.0);
                colorIdx.push_back(k);
            }

        } // end k loop


        // draw the data between these two coordinates, scaling width to count
        for (int k = 0; k < frequency.size(); k++)
        {
            vector<double>* color = pc->getPointColor(colorIdx[k]);
            double alpha = min(1.0, (frequency[k] / (pc->getPointAmount() / 8)));
            // double alpha = 1.0;
            glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
            double width = 1.0 + (2.0 * min(2.0, (frequency[k] / (pc->getPointAmount() / 8))));
            // double width = 3.0;
            glLineWidth(width);
            double leftData = leftCoordinate[k];
            double rightData = rightCoordinate[k];

            // draw a line using both points
            glBegin(GL_LINE_STRIP);
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (leftData * (worldHeight * 0.5)) + (0.175 * worldHeight));
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 2)),
                (rightData * (worldHeight * 0.5)) + (0.175 * worldHeight));
            glEnd();
        }
        coordinateCount++;
    } // end i loop
    glFlush();
}

void drawFrequencyBlock(ParallelCoordinates * pc, double worldWidth, double worldHeight, int blockIdx) {
    vector<double> leftCoordinate = vector<double>();
    vector<double> rightCoordinate = vector<double>();
    vector<double> frequency = vector<double>();
    vector<int> colorIdx = vector<int>();
    bool alreadyExists = false;

    int coordinateCount = 0;
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);

    glColor4d(0.0, 0.0, 0.0, 1.0);

    int split = 3;
    double quantileSize = DBL_MAX;
    Hyperblock currBlock = pc->getBlocks()[blockIdx];

    // find the smallest quantile
    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        double max = currBlock.getMaximum(i);
        double min = currBlock.getMinimum(i);
        double span = max - min;
        if (max != min)
            quantileSize = min(quantileSize, (span / (split * 1.0)));
    }

    vector<int> quantileCount = vector<int>();
    for (int i = 0; i < split; i++) quantileCount.push_back(0);

    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        if (!pc->getDataCoordinates()->at(i)->isVisible())
            continue;

        double max = currBlock.getMaximum(i);
        double min = currBlock.getMinimum(i);
        double span = max - min;

        split = (int) round(span / quantileSize) + 1;

        for (int k = 1; k <= split; k++)
        {
            double ceil = (quantileSize * k) + min;
            double floor = (quantileSize * (k - 1)) + min;
            int count = 0;

            for (int j = 0; j < currBlock.getPointNumber(); j++)
            {
                if (pc->getData(currBlock.getPoints()->at(j), i) <= ceil &&
                    pc->getData(currBlock.getPoints()->at(j), i) >= floor)
                {
                    count++;
                }
            }

            if (count == 1) count = 2;

            glColor4d(255.0/255.0, 255.0/255.0, 255.0/255.0, 1.0);
            glBegin(GL_QUADS);

            // draw bottom left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) - ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) - ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();

            // draw line to seperate quantiles

            glColor4d(0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1.0);
            glBegin(GL_LINE_STRIP);

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) - ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * (count / 2)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();
        } // end k loop

        coordinateCount++;
    } // end coordinate loop

    coordinateCount = 0;

    // create array of coordinates and counts
    for (int i = 0; i < pc->getCoordinateAmount() - 1; i++)
    {
        if (!(pc->getDataCoordinates()->at(i)->isVisible())) continue;
        if (pc->getVisibleCoordinateCount() < 2) break;

        leftCoordinate.clear();
        rightCoordinate.clear();
        frequency.clear();
        colorIdx.clear();
        alreadyExists = false;

        int j = i + 1;
        while (j < pc->getCoordinateAmount() && !(pc->getDataCoordinates()->at(j)->isVisible())) j++;
        if (j >= pc->getCoordinateAmount()) continue;

        for (int k = 0; k < currBlock.getPointNumber(); k++)
        {
            double left = pc->getData(currBlock.getPoints()->at(k), i);
            double right = pc->getData(currBlock.getPoints()->at(k), j);

            // see if exists already, if so increment count; else add and count = 1
            alreadyExists = false;
            for (int l = 0; l < frequency.size(); l++)
            {
                if (leftCoordinate[l] == left &&
                    rightCoordinate[l] == right)
                {
                    alreadyExists = true;
                    frequency[l] = frequency[l] + 1.0;
                    break; // break out of l loop
                }
            } // end l loop

            if (!alreadyExists)
            {
                leftCoordinate.push_back(left);
                rightCoordinate.push_back(right);
                frequency.push_back(1.0);
                colorIdx.push_back(k);
            }

        } // end k loop


        // draw the data between these two coordinates, scaling width to count
        for (int k = 0; k < frequency.size(); k++)
        {
            vector<double>* color = pc->getPointColor(currBlock.getPoints()->at(colorIdx[k]));
            double alpha = min(1.0, (frequency[k] / (currBlock.getPointNumber() / 8)));
            // double alpha = 1.0;
            glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
            // glColor4d(0.0, 1.0, 0.0, alpha);
            double width = 1.0 + (2.0 * min(2.0, (frequency[k] / (currBlock.getPointNumber() / 8))));
            // double width = 3.0;
            glLineWidth(width);
            double leftData = leftCoordinate[k];
            double rightData = rightCoordinate[k];

            // draw a line using both points
            glBegin(GL_LINE_STRIP);
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (leftData * (worldHeight * 0.5)) + (0.175 * worldHeight));
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 2)),
                (rightData * (worldHeight * 0.5)) + (0.175 * worldHeight));
            glEnd();
        }
        coordinateCount++;
    } // end i loop
    glFlush();
}

void drawHistogramAllData(ParallelCoordinates * pc, double worldWidth, double worldHeight) {
    int split = 10;
    double quantileSize = DBL_MAX;
    int coordinateCount = 0;
    glLineWidth(3.0);
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);
    glColor4d(0.0, 1.0, 0.0, 1.0);
    
    vector<int> quantileCount = vector<int>();
    for (int i = 0; i < split; i++) quantileCount.push_back(0);

    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        if (!pc->getDataCoordinates()->at(i)->isVisible())
            continue;

        for (int k = 1; k <= split; k++)
        {
            double ceil = (1.0 / split) * k;
            double floor = (1.0 / split) * (k - 1);
            int count = 0;

            for (int j = 0; j < pc->getDataCoordinates()->at(i)->size(); j++)
            {
                if (pc->getDataCoordinates()->at(i)->getData(j) <= ceil &&
                    pc->getDataCoordinates()->at(i)->getData(j) >= floor)
                {
                    count++;
                }
            }

            if (count == 1) count = 2;

            glColor4d(0.0, 255.0, 0.0, 1.0);
            glBegin(GL_QUADS);

            // draw bottom left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / pc->getPointAmount()) * count),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / pc->getPointAmount()) * count),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();

            glColor4d(0.0, 0.0, 0.0, 1.0);
            glBegin(GL_LINE_STRIP);

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / pc->getPointAmount()) * count),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / pc->getPointAmount()) * count),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();
        } // end k loop

        coordinateCount++;
    } // end coordinate loop
    glFlush();
}

void drawHistogramBlock(ParallelCoordinates * pc, double worldWidth, double worldHeight, int blockIdx) {
    int split = 10;
    double quantileSize = DBL_MAX;
    int coordinateCount = 0;
    glLineWidth(3.0);
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);
    glColor4d(0.0, 1.0, 0.0, 1.0);

    Hyperblock currBlock = pc->getBlocks()[blockIdx];

    vector<int> quantileCount = vector<int>();
    for (int i = 0; i < split; i++) quantileCount.push_back(0);


    // find the smallest quantile
    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        double max = currBlock.getMaximum(i);
        double min = currBlock.getMinimum(i);
        double span = max - min;
        if (max != min)
            quantileSize = min(quantileSize, (span / (split * 1.0)));
    }	

    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        if (!pc->getDataCoordinates()->at(i)->isVisible())
            continue;

        double max = currBlock.getMaximum(i);
        double min = currBlock.getMinimum(i);
        double span = max - min;

        split = (int) round(span / quantileSize) + 1;

        for (int k = 1; k <= split; k++)
        {
            double ceil = (quantileSize * k) + min;
            double floor = (quantileSize * (k - 1)) + min;
            int count = 0;

            for (int j = 0; j < currBlock.getPointNumber(); j++)
            {
                if (pc->getData(currBlock.getPoints()->at(j), i) <= ceil &&
                    pc->getData(currBlock.getPoints()->at(j), i) >= floor)
                {
                    count++;
                }
            }

            if (count == 1) count = 2;

            glColor4d(0.0, 255.0, 0.0, 1.0);
            glBegin(GL_QUADS);

            // draw bottom left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * count),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * count),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();

            glColor4d(0.0, 0.0, 0.0, 1.0);
            glBegin(GL_LINE_STRIP);

            // draw top left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw top right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * count),
                (ceil * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom right
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)) + ((xAxisIncrement / currBlock.getPointNumber()) * count),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            // draw bottom left
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)),
                (floor * (worldHeight * 0.5)) + (0.175 * worldHeight));

            glEnd();
        } // end k loop

        coordinateCount++;
    } // end coordinate loop
    glFlush();
}

void drawAllData(ParallelCoordinates * pc, double worldWidth, double worldHeight) {
    int coordinateCount = 0;	
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1);
    for (int j = 0; j < pc->getPointAmount(); j++)
    {
        std::vector<double>* colorOfCurrent = pc->getPointColor(j);
        glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
        
        glBegin(GL_LINE_STRIP); // begins drawing lines
        for (int i = 0; i < pc->getCoordinateAmount(); i++)
        {
            if (pc->getDataCoordinates()->at(i)->isVisible())
            {
                double currentData = pc->getData(j, i);
                glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)), (currentData * (worldHeight * 0.5)) + (0.175 * worldHeight));
            
                coordinateCount++;
            }
        }
        glEnd(); // ends drawing line
        coordinateCount = 0;
    }
    glFlush();
}

void drawCoordinates(ParallelCoordinates * pc, double worldWidth, double worldHeight) {
    int coordinateCount = 0;
    double xAxisIncrement = worldWidth / (pc->getVisibleCoordinateCount() + 1); 
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < pc->getCoordinateAmount(); i++)
    {
        if (pc->getDataCoordinates()->at(i)->isVisible())
        {
            double shiftAmount = pc->getCoordinateShift(i);
            glBegin(GL_LINE_STRIP);
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)), (shiftAmount * (worldHeight * 0.5) + worldHeight * 0.75));
            glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (coordinateCount + 1)), (shiftAmount * (worldHeight * 0.5) + worldHeight * 0.1));
            glEnd();
            coordinateCount++;
        }
    }
    glFlush();
}