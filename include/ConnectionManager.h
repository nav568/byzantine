/*
 * ConnectionManager.h
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_
#include<vector>
#include<string>
class InputParser;
class ConnectionManager {
public:
	ConnectionManager(InputParser *);
	virtual ~ConnectionManager();
	void* receiveAndProcess(void *);
	void waitForConnections();
	std::string getMyHostName();
	void* get_in_addr(struct sockaddr*);
	std::vector<std::string> hostnames;
	enum decision{
		ATTACK, RETREAT
	};
	std::string prettyprint(ConnectionManager::decision);
	void generalSendToAll(ConnectionManager::decision);
	InputParser *parser;
};

#endif /* CONNECTIONMANAGER_H_ */
