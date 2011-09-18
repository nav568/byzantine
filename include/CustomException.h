/*
 * CustomException.h
 *
 *  Created on: Sep 17, 2011
 *      Author: nsomasun
 */

#ifndef CUSTOMEXCEPTION_H_
#define CUSTOMEXCEPTION_H_
#include <sstream>
#include <iostream>
class CustomException : public std::exception
{
    std::string exceptionMessage;
public:
    CustomException(const std::string &userError, const char * function, const char * file, int line) : std::exception()
    {
    	std::stringstream errorMessage;
        errorMessage << file << "--" << function << ":" << line << ": " << userError << std::endl;
        std::cout << errorMessage.str() << std::endl;
        exceptionMessage = errorMessage.str();
    }

    ~CustomException() throw() {}

    /**
     *  The throw statement!
     */
    const char *what() const throw()
    {
        return exceptionMessage.c_str();
    }
};


#define throw_exception(string) throw CustomException(string,__FUNCTION__, __FILE__, __LINE__ );
#endif /* CUSTOMEXCEPTION_H_ */

