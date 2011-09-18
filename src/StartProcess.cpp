/*
 * StartProcess.cpp
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#include "StartProcess.h"
#include "InputParser.h"
#include "CustomException.h"
#include "ConnectionManager.h"
#include <iostream>
using namespace std;

StartProcess::StartProcess() {
	// TODO Auto-generated constructor stub

}

StartProcess::~StartProcess() {
	// TODO Auto-generated destructor stub
}

int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		throw_exception("Invalid number of arguments passed");
	}
	//TODO check for parameters order
	InputParser *parser = new InputParser(argv[1]);
	ConnectionManager *connection = new ConnectionManager(parser->hostnames);
	
	cout << "My host name is: " << connection->getMyHostName() << endl;
	if(0 == connection->getMyHostName().compare(parser->hostnames[0]))
	{
		cout << "I am the general, initiating and sending message to everyone" << endl;
		connection->generalSendToAll(ConnectionManager::ATTACK);	
		return 0;
	}
	
	connection->waitForConnections();
}
