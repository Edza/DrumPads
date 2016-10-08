#!/bin/bash
install_name_tool -change librtmidi.2.1.0.dylib @executable_path/../Frameworks/librtmidi.2.1.0.dylib DrumPads.app/Contents/MacOS/DrumPads
install_name_tool -change /usr/local/lib/libSDL2_mixer-2.0.0.dylib @executable_path/../Frameworks/libSDL2_mixer-2.0.0.dylib DrumPads.app/Contents/MacOS/DrumPads
install_name_tool -change /usr/local/lib/libportaudio.2.dylib @executable_path/../Frameworks/libportaudio.2.dylib DrumPads.app/Contents/MacOS/DrumPads
install_name_tool -change /usr/local/lib/libSDL2-2.0.0.dylib @executable_path/../Frameworks/libSDL2-2.0.0.dylib DrumPads.app/Contents/MacOS/DrumPads
otool -L DrumPads.app/Contents/MacOS/DrumPads
