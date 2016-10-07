# Sigmatizm makefile.  Requires wxWidgets.
# by default, wx-config from the PATH is used
# If this fails, install apt package libwxgtk3.0-dev
WX_CONFIG := wx-config

# Main executable file
PROGRAM = DrumPads

# Directory containing library portions of code.
INCLUDEDIR2 = /usr/include
# To switch to rtmidi 2.1, we need to change the error handling in wxkeyboard.cpp and also find a way to get MIDI devices
# to work, because they don't in 2.1 for some reason. Something to do with the dummy device, according to an error.
INCLUDEDIR = ../../lib/SDL_2-2.0.3/include
INCLUDEDIR3 = ../../lib/rtmidi-2.1.0
INCLUDEDIR4 = ../../lib/SDL2_mixer-2.0.0/include
LIBDIR = /usr/lib

# Object files
#OBJECTS = wxDrumPad.o DrumPads.o DrumPadsApp.o $(INCLUDEDIR3)/RtMidi.o
OBJECTS = wxDrumPad.o DrumPads.o DrumPadsApp.o ../wxAudioControls/wxSettingsFile.o ../wxAudioControls/wxMidiSettingsDlg.o ../wxAudioControls/wxSwitch.o ../wxAudioControls/wxBitmapSpinButton.o ../wxAudioControls/wxKeylessChoice.o

#CXX = $(shell $(WX_CONFIG) --cxx -ggdb)
CXX = $(shell $(WX_CONFIG) --cxx -O3)

.SUFFIXES:	.o .cpp

.cpp.o :
#	$(CXX) -c -O3 -D__LINUX_ALSASEQ__ -I$(INCLUDEDIR) -I$(INCLUDEDIR2) -I$(INCLUDEDIR3) -I$(INCLUDEDIR4) `$(WX_CONFIG) --cxxflags` -o $@ $<
	$(CXX) -c -g -ggdb -D__LINUX_ALSASEQ__ -I$(INCLUDEDIR) -I$(INCLUDEDIR2) -I$(INCLUDEDIR3) -I$(INCLUDEDIR4) `$(WX_CONFIG) --cxxflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) -L$(LIBDIR) `$(WX_CONFIG) --libs` -lportaudio -lasound -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lrtmidi

clean: 
	rm -f *.o $(PROGRAM) $(INCLUDEDIR3)/*.o
