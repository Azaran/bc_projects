# Makefile for client/server example
# see 'man make' for more details
PROJECT          = sixtunnel
PROJECT_SOURCES  = main.cpp

DEFINES         = 

CFLAGS          =-g3 -Wall -Wextra -Wno-unused-function -pedantic -pthread 

LIBRARIES       =-lpthread #-llibrary_name  

CC              = g++ -std=c++11
PROJECT_OBJECTS  = $(PROJECT_SOURCES:.cpp=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

###########################################

.SUFFIXES: .cpp .o

.cpp.o:
		$(CC) $(CFLAGS) -c $<

###########################################

all:		$(PROJECT)

rebuild:	clean all

$(PROJECT):	$(PROJECT_OBJECTS)
		$(CC) $(PROJECT_OBJECTS) $(LDFLAGS) -o $@


###########################################

clean:
	rm -fr core* *~ $(PROJECT_OBJECTS) $(PROJECT) .make.state .sb
