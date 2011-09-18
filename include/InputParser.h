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
class InputParser {
public:
	InputParser(char *);
	virtual ~InputParser();
	std::vector<std::string> hostnames;
};

#endif /* INPUTPARSER_H_ */
