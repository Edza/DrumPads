# Main executable file
PROGRAM = DrumPads
PALMPDK = /opt/PalmPDK

# Directory containing library portions of code.
INCLUDEDIR = /opt/PalmPDK/include/SDL/ 
INCLUDEDIR2 = /opt/PalmPDK/include

# Object files
OBJECTS = Drum.o
LIBDIR = /usr/lib/
#LIBDIR = /opt/PalmPDK/device/lib
#LIBDIR = /opt/PalmPDK/arm-gcc/arm-none-linux-gnueabi/libc/usr/lib/
LIBDIR2 = /opt/PalmPDK/host/lib/

CXX = g++ -ggdb
#CXX = g++

.SUFFIXES:	.o .cpp

.cpp.o :
	$(CXX) -c -I$(INCLUDEDIR) -I$(INCLUDEDIR2) -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) -I$(INCLUDEDIR) -I$(INCLUDEDIR2) -L$(LIBDIR) $(OBJECTS) -Wl,--allow-shlib-undefined -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lrt

clean: 
	rm -f *.o $(PROGRAM)

debian:
	strip DrumPads
	mkdir drumpads
	cp DrumPads drumpads
	cp -R images drumpads
	cp -R samples drumpads
	cp logo.png drumpads
