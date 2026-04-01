CPP      = g++
CPPFLAGS = -Wall -g

TARGETS = TwoPipesTwoChildren TwoPipesThreeChildren dynpipe


all: $(TARGETS)

TwoPipesTwoChildren: TwoPipesTwoChildren.o
	$(CPP) $(CPPFLAGS) -o TwoPipesTwoChildren TwoPipesTwoChildren.o

TwoPipesThreeChildren: TwoPipesThreeChildren.o
	$(CPP) $(CPPFLAGS) -o TwoPipesThreeChildren TwoPipesThreeChildren.o

dynpipe: DynPipe.o
	$(CPP) $(CPPFLAGS) -o dynpipe DynPipe.o

TwoPipesTwoChildren.o: TwoPipesTwoChildren.cpp
	$(CPP) $(CPPFLAGS) -c TwoPipesTwoChildren.cpp

TwoPipesThreeChildren.o: TwoPipesThreeChildren.cpp
	$(CPP) $(CPPFLAGS) -c TwoPipesThreeChildren.cpp

DynPipe.o: DynPipe.cpp
	$(CPP) $(CPPFLAGS) -c DynPipe.cpp

clean:
	rm -f TwoPipesTwoChildren.o TwoPipesThreeChildren.o DynPipe.o $(TARGETS)

