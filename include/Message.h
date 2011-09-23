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
#include <iostream>
#define processes 1024
#define signaturesize 1024
#define free -1
#define used 1
#include<vector>

typedef struct signature{
	char sign[signaturesize];
	int status;
}signature;


struct Message {

	public:
		int from[processes];
		signature sign[processes];
		ConnectionManager::decision decision;

		Message()
		{
			std::cout << "Message created " << std::endl;
			for(int i = 0 ;i< processes ; i++)
			{
				from[i] = free;
				memset(sign[i].sign,'\0',signaturesize);
				sign[i].status = free;
			}
		}

		void setDecision(ConnectionManager::decision decision1)
		{
			decision = decision1;
		}

		void push(int newId)
		{
			for(int i =0; i< processes; i++)
			{
				if(from[i] == free)
				{
					from[i] = newId;
					return;
				}
			}
		}

		int findSender()
		{
			for(int i=processes -1; i >=0 ; i--)
			{
				if(from[i] != free)
					{
						return from[i];
					}
			}

			throw_exception("Message has no sender");
		}

		bool verifySignature()
		{
			return true;
		}

		std::vector<int> getSenderList()
		{

			std::vector<int> toReturn;
			for(int i =0; i< processes; i++)
			{
				if(from[i] !=free)
				{
					toReturn.push_back(from[i]);
				}
			}

			return toReturn;
		}
};

#endif /* MESSAGE_H_ */
