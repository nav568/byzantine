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
class ConnectionManager {
public:
	ConnectionManager(std::vector<std::string>);
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
};

#endif /* CONNECTIONMANAGER_H_ */
