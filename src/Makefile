CC = g++
OBJ = StartProcess.o InputParser.o ConnectionManager.o SignatureManager.o 
EXEC = general
INCLUDE = /homes/nsomasun/Byzantine/include
SRC = /homes/nsomasun/Byzantine/src

all: $(OBJ)
	$(CC) -g  $(OBJ) -o $(EXEC) -lpthread -lcrypto

StartProcess.o: $(INCLUDE)/StartProcess.h InputParser.o StartProcess.cpp 
	$(CC) -g -c $(SRC)/StartProcess.cpp -I$(INCLUDE)

InputParser.o: $(INCLUDE)/InputParser.h $(INCLUDE)/CustomException.h InputParser.cpp
	$(CC) -g -c $(SRC)/InputParser.cpp -I$(INCLUDE)
	
ConnectionManager.o: $(INCLUDE)/ConnectionManager.h $(INCLUDE)/Message.h ConnectionManager.cpp 
	$(CC) -g -c $(SRC)/ConnectionManager.cpp -I$(INCLUDE)

Message.o:$(INCLUDE)/Message.h 
	$(CC) -g -c $(SRC)/Message.cpp -I$(INCLUDE)

SignatureManager.o: $(INCLUDE)/Message.h $(INCLUDE)/SignatureManager.h  SignatureManager.cpp $(INCLUDE)/CustomException.h
	$(CC) -g -c $(SRC)/SignatureManager.cpp -I$(INCLUDE)
	
clean:
	-rm -f $(OBJ) general
