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
	InputParser *parser = new InputParser(argc, argv);
	ConnectionManager *connection = new ConnectionManager(parser);
	
	cout << "My host name is: " << connection->getMyHostName() << endl;
	if(0 == connection->getMyHostName().compare(parser->hostnames[0]))
	{
		if(parser->order.length() == 0)
		{	
			cout << "No order passed to the general" << endl;
			throw_exception("No order passed to the general");
		}

		cout << "I am the general, initiating and sending message to everyone" << endl;

		switch(parser->mode)
		{
			case InputParser::TRAITOR:
				{
					//traitor
					cout << "betraying... >:)" << endl;
					connection->generalSendToAll(ConnectionManager::RETREAT);	
				}
				break;
			case InputParser::LOYAL:
				{
					//loyal
					connection->generalSendToAll(ConnectionManager::ATTACK);	
				}
		}
		return 0;
	}

	connection->waitForConnections();

	delete parser;
	delete connection;
}
