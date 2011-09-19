/*
 * ConnectionManager.cpp
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#include "ConnectionManager.h"
#include "CustomException.h"
#include "InputParser.h"
#include <string>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <error.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <vector>
#include "Message.h"
#define PORT 3434

using namespace std;

class InputParser;
typedef struct threadStructure{
	pthread_t id;
	int index;
	int socket;
	ConnectionManager::decision decision;
	ConnectionManager *connection;
}threadStructure;


ConnectionManager::ConnectionManager(InputParser *parser) {
	this->hostnames = parser->hostnames;
	this->parser = parser;
}


std::string ConnectionManager::prettyprint(ConnectionManager::decision decision)
{
	if(decision == ConnectionManager::ATTACK)
		return "ATTACK";
	else
		return "RETREAT";
}


std::string ConnectionManager::getMyHostName(){


	struct addrinfo hints, *info, *p;
	int gai_result;

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
	}

	string toReturnName(info->ai_canonname);
	for(p = info; p != NULL; p = p->ai_next) {
		//printf("hostname: %s\n", p->ai_canonname);
	}

	return toReturnName;

}

void* receiveAndProcess(void *void_socket)
{
	threadStructure *waitArray = (threadStructure *)void_socket;
	cout << "Receiving message from socket " << waitArray->socket << endl;
	Message msg;
	int recvbytes = recv(waitArray->socket,&msg,sizeof msg, 0);
	cout << "The number of bytes received is " << recvbytes << endl;
	cout << "Received the decision value:" << waitArray->connection->prettyprint(msg.decision)  << " From the host: " << msg.whoIsThis << std::endl;
	vector<string> *temp = &(waitArray->connection->hostnames);
	waitArray->decision = msg.decision;


	//TODO record general's message here
	if(strcmp(msg.whoIsThis,temp->at(0).c_str()) != 0)
	{
		cout << "Message not received from general " << endl;
		return 0;
	}

	cout << "Message received from general, re-broadcasting " << endl;


	struct hostent *structHostent;
	struct sockaddr_in toConnect;
	int sockfd;
	for(int i=1; i < temp->size(); i++)
	{
		int sockfd, numbytes;
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		char portchange[128];
		cout << "Tyring to connect to host: " << temp->at(i) << endl;
		sprintf(portchange, "%s", waitArray->connection->parser->port.c_str());

		if((temp->at(i)).compare(waitArray->connection->getMyHostName()) == 0)
		{
			cout << "Trying to connect to the same host, skipping" << endl;
			continue;
		}

		if ((rv = getaddrinfo(temp->at(i).c_str(), portchange, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 0;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
							p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}
			break;
		}

		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return 0;
		}

		inet_ntop(p->ai_family, waitArray->connection->get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);
		freeaddrinfo(servinfo); // all done with this structure


		switch(waitArray->connection->parser->mode)
		{	
			case InputParser::TRAITOR:
				{
					Message toSend(ConnectionManager::RETREAT,waitArray->connection->getMyHostName());
					if ((numbytes = send(sockfd, (void *)&toSend, sizeof toSend, 0)) == -1) {
						perror("recv");
						return 0;
					}
					break;
				}
			case InputParser::LOYAL:
				{
					//loyal
					Message toSend(msg.decision,waitArray->connection->getMyHostName());
					if ((numbytes = send(sockfd, (void *)&toSend, sizeof toSend, 0)) == -1) {
						perror("recv");
						return 0;
					}
				}
		}
		close(sockfd);
	}
}


void ConnectionManager::generalSendToAll(ConnectionManager::decision decision)
{
	for(int i=1; i < hostnames.size(); i++)
	{
		cout << "Trying to connect to host: " << hostnames[i] << endl;
		int sockfd, numbytes;
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		char portchange[128];
		sprintf(portchange, "%s", parser->port.c_str());
		cout << "The port/service is " << portchange << endl;

		if ((rv = getaddrinfo(hostnames[i].c_str(), portchange, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
							p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}
			break;
		}

		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return;
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);
		freeaddrinfo(servinfo); // all done with this structure

		switch(parser->mode)
		{
			case InputParser::TRAITOR:
				{
					cout << "Betraying... >:) " << endl;
					//traitor
					Message msg(ConnectionManager::RETREAT,getMyHostName());
					if ((numbytes = send(sockfd, (void *)&msg, sizeof msg, 0)) == -1) {
						perror("recv");
						return;
					}
				}
				break;
			case InputParser::LOYAL:
				{
					//loyal
					Message msg(decision,getMyHostName());
					if ((numbytes = send(sockfd, (void *)&msg, sizeof msg, 0)) == -1) {
						perror("recv");
						return;
					}
				}
		}


		close(sockfd);
	}
}

void *ConnectionManager::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



static void threadHelper(threadStructure *tempStruct)
{
	pthread_create(&(tempStruct->id), NULL, receiveAndProcess,((void *)tempStruct));
}


void ConnectionManager::waitForConnections()
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	char portchange[128];
	sprintf(portchange, "%s", parser->port.c_str());

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	if ((rv = getaddrinfo(hostname, portchange, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
			perror("setsockopt");
			return;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, 10) == -1) {
		perror("listen");
		return;
	}

	printf("server: waiting for connections...\n");

	threadStructure *waitArray = new threadStructure[hostnames.size()];
	for(int i=0;i<hostnames.size() - 1; i++) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
		printf("server: got connection from %s\n", s);
		waitArray[i].index = i;
		waitArray[i].socket = new_fd;
		waitArray[i].connection = this;
		threadHelper(&(waitArray[i]));
	}

	for(int i=0;i<hostnames.size() - 1; i++) {  // main accept() loop
		pthread_join(waitArray[i].id,NULL);
	}


	ConnectionManager::decision result = waitArray[0].decision;

	for(int i=0;i<hostnames.size() - 1; i++) {  // main accept() loop

		if(waitArray[i].decision != waitArray[0].decision)
		{

			cout << "Choosing the default decision" << endl;
			result = RETREAT;
			break;
		}
	}	

	cout << "The decision reached is: " << prettyprint(result) << endl;

	delete[] waitArray;
}

ConnectionManager::~ConnectionManager() {
}
