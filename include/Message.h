/*
 * Message.h
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <cstring>
struct Message {

public:
	Message()
	{

	}
	Message(ConnectionManager::decision decision, std::string toCopy){
		this->decision = decision;
		memcpy(whoIsThis, (char *)toCopy.c_str(), toCopy.length());
		whoIsThis[toCopy.length()] = '\0';
	}
	char whoIsThis[2048];
	ConnectionManager::decision decision;
};

#endif /* MESSAGE_H_ */
