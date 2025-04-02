/**
 * A library of algorithms that use parallel coordinates.
 */

#include "ParallelCoordinates.h"

// Automated machine learning algorithm to create hyperblocks from the parallel coordinates data 
void autoHyperblock(ParallelCoordinates * pc) {
	double accThreshold = 0.90;
	vector<int> selectedPoints = vector<int>();
	vector<Hyperblock> blocks = vector<Hyperblock>();

    // Create a hyperblock for every point
	for (int i = 0; i < pc->getPointAmount(); i++)
	{
		selectedPoints.clear();
		selectedPoints.push_back(i);

		Color blockColor = Color();
		std::vector<double>* colorComponents = pc->getClasses()[pc->getClassOfPoint(i)].getColor();
		blockColor.setRed((*colorComponents)[0]);
		blockColor.setGreen((*colorComponents)[1]);
		blockColor.setBlue((*colorComponents)[2]);
		blockColor.setAlpha((*colorComponents)[3]);
		blocks.push_back(Hyperblock(blockColor, &selectedPoints, pc->getDataCoordinates()));
		blocks[blocks.size() - 1].setRadius(0.0);
		string name = to_string(i);
		blocks[blocks.size() - 1].setName(&name);
		blocks[blocks.size() - 1].setOriginalPoint(i);
	}


    // combine into only pure blocks
	bool actionTaken = false;
	set<int> toBeDeleted = set<int>();
	int count = blocks.size();
	do
	{
		if (count <= 0)
		{
			count = blocks.size();
		}

		toBeDeleted.clear();
		actionTaken = false;

		if (blocks.size() <= 0)
			break;

		Hyperblock temp = blocks.front();

		blocks.erase(blocks.begin());

		int tempClass = pc->getClassOfPoint(temp.getPoints()->at(0));

		for (int i = 0; i < blocks.size(); i++)
		{
			int currBlockClass = pc->getClassOfPoint(blocks[i].getPoints()->at(0));
			if (tempClass != currBlockClass)
			{
				continue;
			}

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < pc->getCoordinateAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[i].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[i].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < pc->getPointAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < pc->getCoordinateAmount(); k++)
				{
					double currData = pc->getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			set<int> classCount = set<int>();

			// check if new space is pure
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				int currClass = pc->getClassOfPoint(pointsInSpace.at(j));

				if (classCount.find(currClass) != classCount.end())
				{
					continue;
				}

				classCount.insert(currClass);
			}

			// if pure then combine the two cubes
			if (classCount.size() <= 1)
			{
				actionTaken = true;

				temp.getPoints()->clear();
				for (int j = 0; j < pointsInSpace.size(); j++)
				{
					temp.addPoint(pointsInSpace.at(j));
				}
				temp.calculateValues(pc->getDataCoordinates());

				// store this index, to delete the block that was combined
				toBeDeleted.insert(i);
			}
		}

		int offset = 0;
		for (auto x : toBeDeleted)
		{
			blocks.erase(blocks.begin() + (x - offset));
			offset++;
		}

		blocks.push_back(temp);

		count--;

	} while (actionTaken || count > 0);

	// add impurity to pure blocks up to given percentage
	actionTaken = false;
	toBeDeleted = set<int>();
	count = blocks.size();
	do
	{
		if (count <= 0)
		{
			count = blocks.size();
		}

		toBeDeleted.clear();
		actionTaken = false;

		if (blocks.size() <= 0)
			break;

		Hyperblock temp = blocks.front();

		blocks.erase(blocks.begin());

		vector<double> acc = vector<double>();
		for (int i = 0; i < blocks.size(); i++)
		{
			// get majority class
			int majorityClass = 0;
			map<int, int> classCount = map<int, int>();
			for (int j = 0; j < blocks[i].getPointNumber(); j++)
			{
				int currClass = pc->getClassOfPoint(blocks[i].getPoints()->at(j));
				if (classCount.find(currClass) != classCount.end())
				{
					classCount[currClass] += 1;
				}
				else
				{
					classCount[currClass] = 1;
				}
			}

			int majorityCount = INT_MIN;
			for (auto entry : classCount)
			{
				if (entry.second > majorityCount)
				{
					majorityCount = entry.second;
					majorityClass = entry.first;
				}
			}

			int currBlockClass = pc->getClassOfPoint(blocks[i].getPoints()->at(0));

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < pc->getCoordinateAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[i].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[i].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < pc->getPointAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < pc->getCoordinateAmount(); k++)
				{
					double currData = pc->getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			classCount.clear();

			// check if new space is within acceptable range of purity
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				int currClass = pc->getClassOfPoint(pointsInSpace.at(j));

				if (classCount.find(currClass) != classCount.end())
				{
					classCount[currClass] += 1;
				}
				else
				{
					classCount[currClass] = 1;
				}
			}

			double currClassTotal = 0;
			double classTotal = 0;
			for (auto entry : classCount)
			{
				if (entry.first == majorityClass)
				{
					currClassTotal = entry.second;
				}

				classTotal += entry.second;
			}

			acc.push_back(currClassTotal / classTotal);
		}

		int highestAccIdx = 0;
		for (int j = 0; j < acc.size(); j++)
		{
			if (acc[j] > acc[highestAccIdx])
			{
				highestAccIdx = j;
			}
		}

		// if highest accuracy is within acceptable range
		if (acc[highestAccIdx] >= accThreshold)
		{
			actionTaken = true;

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < pc->getCoordinateAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[highestAccIdx].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[highestAccIdx].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < pc->getPointAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < pc->getCoordinateAmount(); k++)
				{
					double currData = pc->getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			temp.getPoints()->clear();
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				temp.addPoint(pointsInSpace.at(j));
			}
			temp.calculateValues(pc->getDataCoordinates());

			// store this index, to delete the block that was combined
			toBeDeleted.insert(highestAccIdx);
		}

		int offset = 0;
		for (auto x : toBeDeleted)
		{
			blocks.erase(blocks.begin() + (x - offset));
			offset++;
		}

		blocks.push_back(temp);

		count--;

	} while (actionTaken || count > 0);

	for (int i = 0; i < blocks.size(); i++)
	{
		pc->getBlocks().push_back(blocks.at(i));
        string name = to_string(i);
		pc->getBlocks()[pc->getBlockAmount() - 1].setName(&name);
	}
}

// Automatically colors the classes in the parallel coordinates data
void autoColor(ParallelCoordinates * pc) {
    double red = 1;
	double green = 1;
	double blue = 0;
	double step = 1.0 / ((pc->getClassAmount() - 1) / 3);
	std::vector<vector<double>>startingColors = std::vector<vector<double>>();
	for (int i = 0; i < pc->getClassAmount() - 1; i++)
	{
		startingColors.push_back(std::vector<double>());
		startingColors[i].push_back(red);
		startingColors[i].push_back(green);
		startingColors[i].push_back(blue);
		startingColors[i].push_back(1.0);

		if ((pc->getClassAmount() - 1) < 3)
		{
			red = 1.0;
			green = 0.0;
			blue = 1.0;
		}
		else if (i < ((pc->getClassAmount() - 1) / 3)) // reds
		{
			if (green >= 0.4)
			{
				green -= step;
				if (green <= 0.4)
				{
					blue = 0.6;
				}
			}
			else
			{
				blue += step;
			}
			
			if (i == ((pc->getClassAmount() - 1) / 3) - 1)
			{
				red = 1.0;
				green = 0.0;
				blue = 1.0;
			}
		}
		else if (i < (((pc->getClassAmount() - 1) / 3) * 2)) // blues
		{
			if (red >= 0.4)
			{
				red -= step;
				if (red <= 0.4)
				{
					green = 0.6;
				}
			}
			else
			{
				green += step;
			}
			
			if (i == (((pc->getClassAmount() - 1) / 3) * 2) - 1)
			{
				red = 0.0;
				green = 1.0;
				blue = 1.0;
			}
		}
		else if (i < (pc->getClassAmount() - 1)) // greens
		{
			if (blue >= 0.4)
			{
				blue -= step;
				if (blue <= 0.4)
				{
					red = 0.6;
				}
			}
			else
			{
				red += step;
			}
		}
	}

	for (int i = 1; i < pc->getClassAmount(); i++)
	{
		pc->getClasses()[i].setColor(startingColors[i - 1]);
	}
}