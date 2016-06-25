# Main executable file
PROGRAM = DrumPads

# Directory containing library portions of code.
INCLUDEDIR = /usr/include/SDL2

# Object files
OBJECTS = Drum.o
LIBDIR = /usr/lib/

CXX = g++ -ggdb
#CXX = g++

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c -I$(INCLUDEDIR) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) -I$(INCLUDEDIR) -L$(LIBDIR) $(OBJECTS) -Wl,--allow-shlib-undefined -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lrt

clean: 
	rm -f *.o $(PROGRAM)

debian:
	strip DrumPads
	mkdir drumpads
	cp DrumPads drumpads
	cp -R images drumpads
	cp -R samples drumpads
	cp logo.png drumpads
