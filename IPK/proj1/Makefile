# Makefile for client/server example
# see 'man make' for more details
SERVER          = server
CLIENT          = client
SERVER_SOURCES  = server.cc
CLIENT_SOURCES  = client.cc

DEFINES         = 

CFLAGS          =  -g3 -Wall -Wno-unused-function

LIBRARIES       = #-llibrary_name

CC              = g++ -std=c++11
SERVER_OBJECTS  = $(SERVER_SOURCES:.cpp=.o)
CLIENT_OBJECTS  = $(CLIENT_SOURCES:.cpp=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

###########################################

.SUFFIXES: .cc .o

.cc.o:
		$(CC) $(CFLAGS) -c $<

###########################################

all:		$(SERVER) $(CLIENT)

rebuild:	clean all

$(SERVER):	$(SERVER_OBJECTS)
		$(CC) $(SERVER_OBJECTS) $(LDFLAGS) -o $@

$(CLIENT):	$(CLIENT_OBJECTS)
		$(CC) $(CLIENT_OBJECTS) $(LDFLAGS) -o $@

###########################################

clean:
	rm -fr core* *~ $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(SERVER) $(CLIENT) .make.state .sb
