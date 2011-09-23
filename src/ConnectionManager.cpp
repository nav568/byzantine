/*
 * ConnectionManager.cpp
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */


#include <cstdlib>
#include <stdio.h>
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
#include <set>
#include <map>

using namespace std;

class InputParser;
typedef struct threadStructure{
	pthread_t id;
	int index;
	int socket;
	ConnectionManager *connection;
}threadStructure;


ConnectionManager::ConnectionManager(InputParser *parser) {
	this->hostnames = parser->hostnames;
	this->parser = parser;
	isGeneralTraitor = false;

	parser->myId = -1;
	string myHostName = getMyHostName();
	//find the id
	for(int i=0;i<parser->hostnames.size();i++)
	{
		if(parser->hostnames[i].compare(myHostName) == 0)
		{
			parser->myId = i;
			break;
		}
	}

	cout << "My id is " << parser->myId << endl;
	if(parser->myId == -1)
		throw_exception("Hostname not found in the input file");

	pthread_mutex_init(&mutex,NULL);
}


std::string ConnectionManager::prettyprint(ConnectionManager::decision decision)
{
	if(decision == ConnectionManager::ATTACK)
		return "ATTACK";
	else
		return "RETREAT";
}

std::string ConnectionManager::prettyprintHostname(int id)
{
	if(parser->hostnames.size() < id)
		throw_exception("The index passed to the vector is out of bounds, id passed");

	return hostnames[id];
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

	int recvbytes = 0;
	char *seekWriter = (char *)&msg;


	while(recvbytes != sizeof msg)
	{
		int temp =recv(waitArray->socket,seekWriter,sizeof msg, 0);
		seekWriter+=temp;
		recvbytes += temp;
	}

	//int recvbytes = recv(waitArray->socket, &msg, sizeof msg, MSG_WAITALL);
	//cout << "The number of bytes received is " << recvbytes << endl;

	for(int i=0;i<4;i++)
	{
		cout << "From[" << i << "] <=> " << msg.from[i] << endl;
	}

	cout << "Received the decision value:" << waitArray->connection->prettyprint(msg.decision)  << " From the host: " << waitArray->connection->prettyprintHostname(msg.findSender()) << "whose id is " << msg.findSender() <<  std::endl;


	//verify the message
	if(!msg.verifySignature())
	{
		cout << "The message has a forged signature, hence dropping the message" << endl;
		return NULL;
	}

	//check if the message is in the set
	pair<set<ConnectionManager::decision>::iterator, bool> ret;
	pthread_mutex_lock(&(waitArray->connection->mutex));
	ret = waitArray->connection->receivedSet.insert(msg.decision);
	pthread_mutex_unlock(&(waitArray->connection->mutex));
	if(ret.second == false)
	{
		//already in set, so return;
		return NULL;
	}

	//get the sender list
	std::vector<int> sendList = msg.getSenderList();
	std::vector<int> newList;

	cout << "The message was from the list of: ";
	for(int i=0;i< sendList.size() ; i++)
	{
		cout << sendList[i] << "-->";
	}
	cout << endl;


	//create hash map(host:already_received) and insert all hostnames
	map<int,bool> receivedMap;

	cout << "Setting self-id as received:" <<  waitArray->connection->parser->myId << endl;

	for(int i = 0;i< waitArray->connection->hostnames.size();i++)
	{
		receivedMap[i] = false;
	}

	receivedMap[waitArray->connection->parser->myId] = true;
	
	for(int i=0;i<sendList.size();i++)
	{
		receivedMap[sendList[i]] = true;
	}

	for(int i = 0;i< waitArray->connection->hostnames.size();i++)
	{
		if(receivedMap[i] == false)
		{
			newList.push_back(i);
		}
	}

	vector<int> *temp = &(newList);

	cout << "The message is sent to the following people: ";
	for(int i=0;i< temp->size() ; i++)
	{
		cout << temp->at(i) << "-->";
	}
	cout << endl;



	struct hostent *structHostent;
	struct sockaddr_in toConnect;
	int sockfd;
	for(int i=0; i < temp->size(); i++)
	{
		srand ( time(NULL) );
		
		int sockfd, numbytes;
		struct addrinfo hints, *servinfo, *p;
		int rv;
		char s[INET6_ADDRSTRLEN];
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		char portchange[128];
		
		if(rand()%5 == 1 && waitArray->connection->parser->mode == InputParser::TRAITOR )
		{
			cout << "I am betraying >:) .. " << endl;
			cout << "Not sending anything anything " << waitArray->connection->prettyprintHostname(temp->at(i)) << endl;
			continue;
		}

		cout << "Tyring to connect to host: " << waitArray->connection->prettyprintHostname(temp->at(i)) << endl;
		sprintf(portchange, "%s", waitArray->connection->parser->port.c_str());

		if((temp->at(i)) == waitArray->connection->parser->myId )
		{
			cout << "Trying to connect to the same host, skipping" << endl;
			continue;
		}

		if ((rv = getaddrinfo(waitArray->connection->prettyprintHostname(temp->at(i)).c_str(), portchange, &hints, &servinfo)) != 0) {
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
					cout << "I am betraying >:) .. " << endl;
					switch(rand()%2)
					{
						case 0:
							msg.setDecision(ConnectionManager::ATTACK);
							break;
						case 1: 
							msg.setDecision(ConnectionManager::RETREAT);
					}
					
					msg.push(waitArray->connection->parser->myId);
					if ((numbytes = send(sockfd, (void *)&msg, sizeof msg, 0)) == -1) {
						perror("recv");
					}
				}
				break;
			case InputParser::LOYAL:
				{
					//loyal
					msg.push(waitArray->connection->parser->myId);
					if ((numbytes = send(sockfd, (void *)&msg, sizeof msg, 0)) == -1) {
						perror("recv");
					}
				}
		}

		cout << "Sent to the " << waitArray->connection->prettyprintHostname(temp->at(i)) << endl;
		close(sockfd);
	}
}


void ConnectionManager::generalSendToAll(ConnectionManager::decision decision)
{
	for(int i=1; i < hostnames.size(); i++)
	{
		cout << "Trying to connect to host: " << hostnames[i] << endl;
	
		if(rand()%5 == 1 && parser->mode == InputParser::TRAITOR)
		{
			cout << "I am betraying >:) .. " << endl;
			cout << "Not sending anything to host: " << prettyprintHostname(i) << endl;
			continue;
		}

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
					Message msg;
					msg.push(parser->myId);
					switch(rand()%2)
					{
						case 0:
							msg.setDecision(ConnectionManager::ATTACK);
							break;
						case 1: 
							msg.setDecision(ConnectionManager::RETREAT);
					}


					cout << "Issuing command to: "<< prettyprint(msg.decision) << endl;
					if ((numbytes = send(sockfd, (void *)&msg, sizeof msg, 0)) == -1) {
						perror("recv");
						return;
					}
				}
				break;
			case InputParser::LOYAL:
				{
					//loyal
					Message msg;
					msg.push(parser->myId);
					msg.setDecision(decision);
					cout << "Issuing command to: "<< prettyprint(msg.decision) << endl;
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

	//threadStructure *waitArray = new threadStructure[hostnames.size()];
	vector<threadStructure *> waitVector;

	fd_set master;
	FD_ZERO(&master);
	FD_SET(sockfd,&master);
	int max = sockfd;

	while(1) {  

		struct timeval timeout;
		timeout.tv_sec=15;
		timeout.tv_usec=0;

		fd_set reads;
		FD_ZERO(&reads);
		reads = master;

		int result = 0;
		if((result = select(max+1, &reads, NULL, NULL,&timeout)) == -1)
		{
			throw_exception("Error in select statement");
		}

		if(FD_ISSET(sockfd,&reads))
		{
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
			threadStructure *waitArray = new threadStructure;
			waitArray->socket = new_fd;
			waitArray->connection = this;
			threadHelper(waitArray);
			waitVector.push_back(waitArray);
		}
		else
		{
			//Time out reached
			cout << "Reached timeout, deciding now......." << endl;
			break;
		}
	}

	for(int i=0;i<waitVector.size() ; i++) {  
		pthread_join(waitVector[i]->id,NULL);
		delete waitVector[i];
	}


	ConnectionManager::decision result;
	if(receivedSet.size()==0 | receivedSet.size() >= 2)
	{

		cout << "Choosing the default decision" << endl;
		result = RETREAT;
	}
	else
	{
		result = *(receivedSet.begin());
	}


	cout << "The decision reached is: " << prettyprint(result) << endl;

}

ConnectionManager::~ConnectionManager() {
	pthread_mutex_destroy(&mutex);
}
