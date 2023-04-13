SHELL = /bin/sh
NAME = all
MAKEFILE = Makefile

CONVDIR=Binary_Conversion
COOKDIR=Cooking
DARKDIR=Dark

all: 
	cd $(CONVDIR) && $(MAKE) clean && $(MAKE)
	cd $(DARKDIR) && $(MAKE) clean && $(MAKE)
	cd $(COOKDIR) && $(MAKE) realclean && $(MAKE)
clean:
	cd $(CONVDIR) && $(MAKE) clean
	cd $(DARKDIR) && $(MAKE) clean
	cd $(COOKDIR) && $(MAKE) realclean
