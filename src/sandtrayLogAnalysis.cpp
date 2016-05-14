#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <string.h>
#include <dirent.h>


std::vector<std::string> Split (const std::string&, char);
int TimeInSeconds (std::string);
double MeanOfInts (std::vector<int>);
double MeanOfDoubles (std::vector<double>);
int TotalInts (std::vector<int>);
double SDofInts (std::vector<int>);
double SDofDoubles (std::vector<double>);


int main(int argc, char *argv[])
{
	std::string PATH;
	std::string OUTFILE

	if ((argc < 3) || (argc > 3))
	{
		//not enough arguments
		std::cout << "Not correct number of arguments - expect 2:" << std::endl;
		std::cout << "1:\t \"input/directory/path/\" " << std::endl;
		std::cout << "2:\t \"output-file.dat\" " << std::endl;
	}
	else
	{
		PATH.assign(argv[1], (int)argv[1].size());
		OUTFILE.assign(argv[2], (int)argv[2].size());
	}

	printf("\n");
	printf("=================================\n");
	printf(" Sandtray Log Analysis Programme\n");
	printf("         P. Baxter, 2016\n");
	printf("=================================\n");
	printf("\n");

	//eventually replace following with user selection...
	//std::string PATH = "C:/Users/pebaxter/Dropbox/RESULTS/2015-06 WoZ sandtray therapy/child 1/session 6/";
	//PATH = "C:/Users/pebaxter/Documents/Dropbox/RESULTS/2015-06 WoZ sandtray therapy/L.C/ados TT/";				//test pathname
	//std::string OUTPATH = "C:/Users/pebaxter/Documents/Dropbox/RESULTS/2015-06 WoZ sandtray therapy/L.C/";

	std::string OUTPATH = PATH;

	//eventually replace following with child identifier...
	//OUTFILE = "ados-session-data.dat";


	////////////////////////////////////////
	// PROCESSING THRESHOLDS
	int MAX_REACTION = 60;		//max time to wait (in s) before considering a reaction time to a prompt to be too large


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 1
	//	-- get list of filenames in the assigned directory
	std::vector<std::string> fileNames;
	fileNames.clear();
	int fileCount = 0;		//number of files in the directory

	unsigned char isFile = 0x8;		//use 0x4 for finding folders
	DIR *Dir;
	struct dirent *DirEntry;
	Dir = opendir(PATH.c_str());

	if (Dir)
	{
		std::cout << "Directory found: " << std::endl;
		std::cout << "\t" << PATH << std::endl;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Cannot open directory: " << std::endl;
		std::cout << "\t" << PATH << std::endl;
		std::cout << std::endl;
		return 1;
	}

	while ((DirEntry = readdir(Dir)))
	{
		if (DirEntry->d_type == isFile)
		{
			std::cout << "Found file: " << DirEntry->d_name << std::endl;
			fileNames.push_back(DirEntry->d_name);
			fileCount++;
		}
	}
	std::cout << "Total number of files found: " << fileCount << std::endl;
	std::cout << std::endl;

	//if no files, then don't bother continuing
	if (fileCount == 0)
	{
		std::cout << "No files found in selected directory, end of programme..." << std::endl;
		std::cout << std::endl;
		return 1;
	}

	//add a check to remove all non-text files
	std::string extension = ".txt";
	std::vector<int> toRemove;		//list to remove from file list
	toRemove.clear();
	for (int aa = 0; aa < (int)fileNames.size(); aa++)
	{
		//iterate through file names
		std::size_t found = fileNames[aa].find(extension);
		if (found == std::string::npos)
		{
			//substring not found, add to delete list
			toRemove.push_back(aa);
		}
	}
	//now remove the elements: START FROM BACK!
	if ((int)toRemove.size() > 0)
	{
		for (int aaa = ((int)toRemove.size() - 1); aaa >= 0; aaa--)
		{
			fileNames.erase(fileNames.begin() + toRemove[aaa]);
		}
	}
	//final check of file names to see what the contents of the file list is now
	std::cout << "New contents of file list:" << std::endl;
	for (int bb = 0; bb < (int)fileNames.size(); bb++)
	{
		std::cout << fileNames[bb] << std::endl;
	}
	std::cout << std::endl;


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 1a
	//	-- set up variables to handle all session data across multiple files
	//data collection
	int goodChildMoves = 0;
	int badChildMoves = 0;
	int goodRobotMoves = 0;
	int badRobotMoves = 0;
	int startTime = 0;
	int endTime = 0;
	int timeOfLastPrompt = 0;
	int lengthTime = 0;
	int numYourTurn = 0;
	int numMyTurn = 0;
	int numGoodFeedback = 0;
	int numBadFeedback = 0;
	int numLibraries = 0;
	//data containers
	std::vector<int> reactionTime;		//time between robot prompt and child move
	reactionTime.clear();
	std::vector<double> moveSpeeds;	//speed of child moves
	moveSpeeds.clear();
	std::vector<double> turnTaking;	//for each child move, whether turn taking happened
	turnTaking.clear();
	//more data containers
	std::vector<int> sectionTimes;
	sectionTimes.clear();
	//loop variables
	std::string readLine = "";
	bool firstFlag = true;
	bool endFlag = false;
	bool promptFlag = false;
	bool robotLastMove = false;		//if robot was last to move, then this is true
	int lineCounter = 0;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 1b
	//	-- variable names for handling different file names
	std::ifstream fileStream;										//filestream to read from
	char charFileName[512];										//very large just in case...
	std::string fileName = "";										//name of the file to read from


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 2
	//	-- given each of the files in the directory in turn, open and process

	for (int _counter = 0; _counter < fileCount; _counter++)
	{
		fileName = PATH + fileNames[_counter];				//just the first file for now - increment for more than one file
		char *a = new char[fileName.size()+1];
		a[fileName.size()] = 0;
		memcpy(a, fileName.c_str(), fileName.size());
		strcpy (charFileName, a);
		fileStream.open(charFileName, std::fstream::in);				//open file stream for reading
		delete a;		//memory clean-up

		//check file successfully opened before continuing
		readLine = "";
		if (fileStream.is_open())
		{
			std::cout << "*************************************************" << std::endl;
			std::cout << "Opened file: " + fileName << std::endl;
			//read file header
			std::getline(fileStream, readLine);	//first line is just headers
			//only run this check on the first line
			std::vector<std::string> splitLineF = Split(readLine,',');		//split the line read in
			if (splitLineF[0] != "Timestamp")
			{
				//if this is not true, then know that this is not a valid datafile...
				std::cout << "ERROR: this file does not appear to be a valid data file: " + readLine << std::endl;
				fileStream.close();
				continue;		//exit the while, go to next file...
			}
		}
		else
		{
			//some error
			std::cout << "ERROR reading file: " + fileName << std::endl;
			fileStream.close();
			return 1;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//step 3
		//	-- extract data, process, add to global counters, then close file
		//data collection
		startTime = 0;
		endTime = 0;
		timeOfLastPrompt = 0;
		lengthTime = 0;
		//loop variables
		readLine = "";
		firstFlag = true;
		endFlag = false;
		promptFlag = false;
		robotLastMove = false;		//if robot was last to move, then this is true
		lineCounter = 0;
		while (std::getline(fileStream, readLine))
		{
			//read file until end
			std::vector<std::string> splitLine = Split(readLine,',');		//split the line read in

			if (splitLine.size() < 3)		//minimum of three elements to every line read from file
			{
				std::cout << "ERROR: problem reading line from file: " + readLine << std::endl;
				continue;		//skip to next while loop iteration
			}

			if (splitLine[1] == "SandtrayEvent")					//////////////////// SANDTRAY EVENT
			{
				if (splitLine[2] == "i am a touchscreen")
				{
					//std::cout << "Time of touchscreen connect: " + splitLine[0] << std::endl;
				}
				else if (splitLine[2] == "22")
				{		//library change
					numLibraries++;
				}
				else if (splitLine[2] == "50")
				{		//child good move
					if(promptFlag == true)		//if two child moves in a row, then don't count the time to second move...
					{
						int currTime = TimeInSeconds(splitLine[0]);
						int elapsed = currTime - timeOfLastPrompt;
						if (elapsed > MAX_REACTION)
						{
							//if more than a minute has passed, then beware!
							std::cout << "*** Reaction time to robot prompt very high! Value: " << elapsed << std::endl;
							//ignore this value...
						}
						else
						{
							reactionTime.push_back(elapsed);
						}
						promptFlag = false;
					}
					if (splitLine.size() >= 5)
					{		//ensure that the container is at least this big, otherwise index errors
						double speed = atof(splitLine[5].c_str());
						moveSpeeds.push_back(speed);
					}
					if (robotLastMove == true)
					{
						turnTaking.push_back(1.0);		//correct turn-taking
						robotLastMove = false;
					}
					else
					{
						turnTaking.push_back(0.0);		//bad turn-taking
						robotLastMove = false;
					}
					goodChildMoves++;
				}
				else if (splitLine[2] == "51")
				{		//child bad move
					if(promptFlag == true)		//if two child moves in a row, then don't count the time to second move...
					{
						int currTime = TimeInSeconds(splitLine[0]);
						int elapsed = currTime - timeOfLastPrompt;
						if (elapsed > MAX_REACTION)
						{
							//if more than a minute has passed, then beware!
							std::cout << "*** Reaction time to robot prompt very high! Value: " << elapsed << std::endl;
							//ignore this value
						}
						else
						{
							reactionTime.push_back(elapsed);
						}
						promptFlag = false;
					}
					if (splitLine.size() >= 5)
					{		//ensure that the container is at least this big, otherwise index errors
						double speed = atof(splitLine[5].c_str());
						moveSpeeds.push_back(speed);
					}
					if (robotLastMove == true)
					{
						turnTaking.push_back(1.0);		//correct turn-taking
						robotLastMove = false;
					}
					else
					{
						turnTaking.push_back(0.0);		//bad turn-taking
						robotLastMove = false;
					}
					badChildMoves++;
				}
				else if (splitLine[2] == "52")
				{		//robot bad move
					badRobotMoves++;
					robotLastMove = true;		//only change flag after move has finished
				}
				else if (splitLine[2] == "53")
				{		//robot good move
					goodRobotMoves++;
					robotLastMove = true;		//only change flag after move has finished
				}
			}
			else if (splitLine[1] == "SandtrayFeedback")		//////////////////// SANDTRAY FEEDBACK
			{
			}
			else if (splitLine[1] == "GUIcommand")				//////////////////// GUI COMMAND
			{
				if (firstFlag == true)
				{
					if ((splitLine[3] == "your-turn") || (splitLine[3] == "my-turn") || (splitLine[3] == "good") || (splitLine[3] == "bad"))
					{
						//consider this to be the start of the interaction
						startTime = TimeInSeconds(splitLine[0]);
						std::cout << "Time of interaction start: " + splitLine[0] + "\t Seconds: " << startTime << std::endl;
						firstFlag = false;
					}
				}
				else			//not the first time come across a GUIcommand
				{
					//std::cout << "GUI command requested: " + splitLine[3] + "\t at time " + splitLine[0] << std::endl;
					if ((splitLine[2] == "behaviour") && (splitLine[3] == "stop"))
					{
						if (!endFlag)
						{
							//end time calculated outside of line read loop...
							endFlag = true;	//if have reached end, then don't look for more!
						}
					}
					else if ((splitLine[2] == "behaviour") && (splitLine[3] == "your-turn"))
					{
						timeOfLastPrompt = TimeInSeconds(splitLine[0]);
						numYourTurn++;
						promptFlag = true;
						//std::cout << "Robot prompt: " << promptFlag << std::endl;
					}
					else if ((splitLine[2] == "behaviour") && (splitLine[3] == "my-turn"))
					{
						numMyTurn++;
					}
					else if ((splitLine[2] == "behaviour") && (splitLine[3] == "bravo"))
					{
						numGoodFeedback++;
					}
					else if ((splitLine[2] == "behaviour") && (splitLine[3] == "bad"))
					{
						numBadFeedback++;
					}
					else if ((splitLine[2] == "move") && (splitLine[3] == "good"))
					{
						//goodRobotMoves++;
					}
					else if ((splitLine[2] == "move") && (splitLine[3] == "bad"))
					{
						//badRobotMoves++;
					}
				}
			}
			else if (splitLine[1] == "MotorFeedback")			//////////////////// MOTOR FEEDBACK
			{
				//not doing anything with this at the moment
			}

			endTime = TimeInSeconds(splitLine[0]);

			lineCounter++;
		}	//END OF WHILE LOOP

		lengthTime = endTime - startTime;

		if (lineCounter < 4)
		{
			//nothing happens in this file!
			std::cout << "Nothing happens in this file!" << std::endl;
			lengthTime = 0;
		}
		else if (startTime == 0)
		{
			//no event has occurred that is worth calling a start, therefore nothing of note happened!
			std::cout << "No valid start event occurred in this file!" << std::endl;
			lengthTime = 0;
		}

		fileStream.close();

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//step 4
		//	-- calculate overall session stats

		std::cout << "----- End of file detected, total length (s): " << lengthTime << std::endl;

		// -- store data for this session
		sectionTimes.push_back(lengthTime);

	}	//END OF FILE PROCESSING FOR SINGLE FILE

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 5a
	//	-- write session stats to screen
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "TOTAL STATS:" << std::endl;
	std::cout << std::endl;
	std::cout << "Total interaction time: " << TotalInts(sectionTimes) << " sec" << std::endl;
	std::cout << std::endl;
	std::cout << "Num libraries used:\t" << numLibraries << std::endl;
	std::cout << "Num robot prompts:\t" << numYourTurn << std::endl;
	std::cout << "Tot num child moves:\t" << (goodChildMoves + badChildMoves) << std::endl;
	std::cout << "Num good child moves:\t" << goodChildMoves << std::endl;
	std::cout << "Num bad child moves:\t" << badChildMoves << std::endl;
	std::cout << "Num prompts per move:\t" << ((double)numYourTurn / ((double)goodChildMoves + (double)badChildMoves)) << std::endl;
	std::cout << std::endl;
	std::cout << "Num good feedback:\t" << numGoodFeedback << std::endl;
	std::cout << "Num bad feedback:\t" << numBadFeedback << std::endl;
	std::cout << "Num robot good moves:\t" << goodRobotMoves << std::endl;
	std::cout << "Num robot bad moves:\t" << badRobotMoves << std::endl;
	std::cout << std::endl;
	std::cout << "Mean reaction time:\t" << MeanOfInts(reactionTime) << "\t\tSD: " << SDofInts(reactionTime) << "\tN: " << (int)reactionTime.size() << std::endl;
	std::cout << "Mean child move speed:\t" << MeanOfDoubles(moveSpeeds) << "\t\tSD: " << SDofDoubles(moveSpeeds) << "\tN: " << (int)moveSpeeds.size() << std::endl;
	std::cout << "Mean child turn-taking:\t" << MeanOfDoubles(turnTaking) << "\tSD: " << SDofDoubles(turnTaking) << "\tN: " << (int)turnTaking.size() << std::endl;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//step 5a
	//	-- write session stats to file - at the moment one level up from the session data: OUTPATH
	std::string filename = OUTPATH + OUTFILE;
	std::ofstream outFile;
	outFile.open(filename.c_str(), std::fstream::out);
	//header
	outFile << "Session,Interaction time (s),N_libraries,N_prompts,N_childMoves,N_goodChild,N_badChild,N_promptsPerMove,N_goodFeedback,N_badFeedback,N_goodRobot,N_badRobot,reactionTime,sd,n,childSpeed,sd,n,childTurnTaking,sd,n" << std::endl;
	//data
	outFile << "," << TotalInts(sectionTimes) << "," << numLibraries << "," << numYourTurn << "," << (goodChildMoves + badChildMoves) << "," << goodChildMoves << "," << badChildMoves << ","
	<< ((double)numYourTurn / ((double)goodChildMoves + (double)badChildMoves)) << "," << numGoodFeedback << "," << numBadFeedback << "," << goodRobotMoves << "," << badRobotMoves
	<< "," << MeanOfInts(reactionTime) << "," << SDofInts(reactionTime) << "," << (int)reactionTime.size() << "," << MeanOfDoubles(moveSpeeds) << "," << SDofDoubles(moveSpeeds) << ","
	<< (int)moveSpeeds.size() << "," << MeanOfDoubles(turnTaking) << "," << SDofDoubles(turnTaking) << "," << (int)turnTaking.size() << std::endl;
	outFile.close();

	return 0;
}	//END OF MAIN



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
