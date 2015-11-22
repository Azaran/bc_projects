# Makefile for client/server example
# see 'man make' for more details
PROJECT          = sixtunnel
PROJECT_SOURCES  = sixtunnel.cpp 

DEFINES         = 

CFLAGS          =-g3 -Wall -Wextra -Wno-unused-function -pedantic -pthread 

LIBRARIES       =-lpthread #-llibrary_name  

CC              = g++ -std=c++0x
PROJECT_OBJECTS  = $(PROJECT_SOURCES:.cpp=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

DOC		= manual
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

doc:		$(DOC)
$(DOC):$(DOC).tex
	latex $(DOC)
	bibtex $(DOC) 
	latex $(DOC)
	latex $(DOC)
	dvips $(DOC).dvi
	ps2pdf -sPAPERSIZE=a4 $(DOC).ps $(DOC).pdf
	ls | grep -v 'manual.pdf' | grep -v 'manual.tex' | grep -Z manual | xargs rm

###########################################

clean:
	rm -fr core* *~ $(PROJECT_OBJECTS) $(PROJECT) .make.state .sb
