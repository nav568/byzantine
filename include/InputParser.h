/*
 * InputParser.h
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */


#ifndef INPUTPARSER_H_
#define INPUTPARSER_H_
#include<vector>
#include<string>
#include<map>
class InputParser {
public:
	std::string  port, fileToOpen, order;
	InputParser(int, char **);
	virtual ~InputParser();
	std::vector<std::string> hostnames;
	int myId;

	enum mode
	{
		LOYAL, TRAITOR
	};
	mode mode;
};

#endif /* INPUTPARSER_H_ */
