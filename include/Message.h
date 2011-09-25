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
#include "ConnectionManager.h"
#include "CustomException.h"
#define processes 1024
#define signaturesize 4096
#define free -1
#define used 1
#include<vector>

typedef struct signature{
	unsigned char sign[signaturesize];
	int status;
	unsigned int length;
}signature;


typedef struct Message {

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

		int push(int newId)
		{
			for(int i =0; i< processes; i++)
			{
				if(from[i] == free)
				{
					from[i] = newId;
					std::cout << "Setting From[" << i << "]" << " to " << from[i] << std::endl;
					return i;
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
} Message;

#endif /* MESSAGE_H_ */
