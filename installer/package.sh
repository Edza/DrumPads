#!/bin/bash
cp -R ../samples_v2/*.wav DrumPads.app/Contents/Resources/Samples/
codesign -f -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app/Contents/Frameworks/libportaudio.2.dylib --entitlements DrumPads.app/Contents/Resources/Entitlements.plist
codesign -f -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app/Contents/Frameworks/librtmidi.2.1.0.dylib --entitlements DrumPads.app/Contents/Resources/Entitlements.plist
codesign -f -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app/Contents/Frameworks/libSDL2-2.0.0.dylib --entitlements DrumPads.app/Contents/Resources/Entitlements.plist
codesign -f -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app/Contents/Frameworks/libSDL2_mixer-2.0.0.dylib --entitlements DrumPads.app/Contents/Resources/Entitlements.plist
codesign -f -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app --entitlements DrumPads.app/Contents/Resources/Entitlements.plist
#codesign -s "3rd Party Mac Developer Application: Jason Champion (G6PFF85X2R)" DrumPads.app/Contents/Resources/Entitlements.plist
#productbuild --sign "Developer ID Installer: Jason Champion (G6PFF85X2R)" --component DrumPads.app /Applications DrumPads.pkg
productbuild --sign "3rd Party Mac Developer Installer: Jason Champion (G6PFF85X2R)" --component DrumPads.app /Applications DrumPads.pkg
