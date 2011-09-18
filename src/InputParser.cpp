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
using namespace std;
InputParser::InputParser(char *fileToOpen) {
	ifstream hostFile(fileToOpen);
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
