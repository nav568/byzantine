/*
 * InputParser.cpp
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#include "InputParser.h"
#include <string>
#include <fstream>
#include <iostream>
#include "CustomException.h"
using namespace std;
InputParser::InputParser(int argc, char **argv) {
	
	bool modeFound = false;
	for(int i=1;i< argc ;i++)
	{
		string currentOption(argv[i]);
		if(currentOption.compare("-port") == 0)
		{
			port.assign(argv[i+1]);
			cout << "Using the port number:" << port << endl;;
		}
		else if(currentOption.compare("-hostfile") == 0)
		{
			fileToOpen.assign(argv[i+1]);
			cout << "Using the host file:" << fileToOpen << endl;
		}
		else if(currentOption.compare("-loyal") == 0)
		{
			modeFound = true;
			mode = InputParser::LOYAL;
			cout << "The general is in the mode:" << currentOption << endl;
		}
		else if(currentOption.compare("-traitor") == 0 )
		{
			modeFound = true;
			mode = InputParser::TRAITOR;
			cout << "The general is in the mode:" << currentOption << endl;
		}
	}

	order.assign(argv[argc-1]);

	if( !modeFound | port.length() == 0 | fileToOpen.length() == 0)
	{
		cout << "Invalid usage: proj1 -loyal -port 8080 -hostfile pathToHostFile [order]" << endl;
		throw_exception("Invalid parameters");
	}

	ifstream hostFile(fileToOpen.c_str());

	if(!hostFile.is_open())
	{
		throw_exception("Unable to open file");
	}

	string fileopen(fileToOpen);
	cout << " File to open: " << fileopen << endl;
	while(true)
	{
		string hostToInsert;
		getline(hostFile,hostToInsert);
		if(hostFile.eof())
		{
			break;
		}
		hostnames.push_back(hostToInsert);
		cout << "Reading host:"<< hostToInsert<< " into the vector" <<  endl;
	}
}

InputParser::~InputParser() {
	// TODO Auto-generated destructor stub
}
