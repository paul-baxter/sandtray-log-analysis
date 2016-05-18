#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

#include "helperFuncs.h"


std::vector<std::string> Split (const std::string &input, char splitter)
{
	std::vector<std::string> toReturn;
	toReturn.clear();

	std::stringstream ss(input);
	std::string item;
	while (std::getline(ss, item, splitter))
	{
		toReturn.push_back(item);
	}

	//seems as though final element has an extra character...
	int size = toReturn.size();
	std::string element = toReturn[size-1];	//the final element in vector
	element.erase(element.size()-1);
	toReturn[size-1] = element;

	return toReturn;
}

int TimeInSeconds (std::string in)
{
	//to convert the timestamp into a number of seconds
	// -- input format: hh:mm:ss
	std::vector<std::string> HMS = Split(in,':');
	int hour = atoi(HMS[0].c_str());
	int minute = atoi(HMS[1].c_str());
	int second = atoi(HMS[2].c_str());

	int timeReturn = (hour * 3600) + (minute * 60) + second;

	return timeReturn;
}

double MeanOfInts (std::vector<int> in)
{
	//to get mean of a vector of ints
	int size = (int)in.size();
	int total = 0;
	for (int a = 0; a < size; a++)
	{
		total += in[a];
	}
	double mean = ((double)total) / ((double)size);

	return mean;
}

double MeanOfDoubles (std::vector<double> in)
{
	//get mean of a vector of doubles
	double size = (double)in.size();
	double total = 0.0;
	for (int a = 0; a < (int)size; a++)
	{
		total += in[a];
	}
	double mean = (total / size);

	return mean;
}

int TotalInts (std::vector<int> in)
{
	int total = 0;

	for (int a = 0; a < (int)in.size(); a++)
	{
		total += in[a];
	}

	return total;
}

double SDofInts (std::vector<int> in)
{
	//calculate the standard deviation of a set of integers
	double sdReturn = 0.0;
	double mean = MeanOfInts(in);
	double sumDev = 0.0;

	for (int a = 0; a < (int)in.size(); a++)
	{
		sumDev += (((double)in[a] - mean) * ((double)in[a] - mean));
	}

	sdReturn = std::sqrt(sumDev / (double)in.size());

	return sdReturn;
}

double SDofDoubles (std::vector<double> in)
{
	//calculate the standard deviation of a set of doubles
	double sdReturn = 0.0;
	double mean = MeanOfDoubles(in);
	double sumDev = 0.0;

	for(int a = 0; a < (int)in.size(); a++)
	{
		sumDev += ((in[a] - mean) * (in[a] - mean));
	}

	sdReturn = std::sqrt(sumDev / (double)in.size());

	return sdReturn;
}
