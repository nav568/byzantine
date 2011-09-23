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
#include <set>
class InputParser;
struct Message;
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
	std::string prettyprintHostname(int id);
	void generalSendToAll(ConnectionManager::decision);
	void findMyId();
	InputParser *parser;
	bool isGeneralTraitor;
	std::set<ConnectionManager::decision> receivedSet;
	pthread_mutex_t mutex;
};

#endif /* CONNECTIONMANAGER_H_ */
