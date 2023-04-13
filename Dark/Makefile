SHELL = /bin/sh
NAME = all
MAKEFILE = Makefile
CXX=g++

ROOT_FLAG = `root-config --cflags --libs`
LIBRARIES  := $(LIBRARIES) -L$(ROOTSYS)/lib
INCLUDES := $(INCLUDES) -I. -I$(ROOTSYS)/include

DIR=.
SRC=$(DIR)/dark.cc
EXECUTABLE=$(DIR)/dark

all: 
	$(CXX) $(SRC) -o $(EXECUTABLE) $(INCLUDES) $(LIBRARIES) $(ROOT_FLAG)
clean:
	rm -rf $(EXECUTABLE)
