SHELL = /bin/sh
NAME = all
MAKEFILE = Makefile
CXX=g++

ROOT_FLAG = `root-config --cflags --libs`
LIBRARIES  := $(LIBRARIES) -L$(ROOTSYS)/lib
INCLUDES := $(INCLUDES) -I. -I$(ROOTSYS)/include

DIR=.
SRC=$(DIR)/dat_to_root.cpp
EXECUTABLE=$(DIR)/dat_to_root

all: 
	$(CXX) $(SRC) -o $(EXECUTABLE) $(INCLUDES) $(LIBRARIES) $(ROOT_FLAG)
clean:
	rm -rf $(EXECUTABLE)
