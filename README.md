# DrumPads

A set of virtual sample-based drum pads that can be played with the keyboard,
mouse, or touchscreen. Works on Windows and Linux, and maybe on OSX.

![DrumPads Screenshot](https://github.com/Xangis/DrumPads/blob/master/images/DrumPads2ScreenshotWithArrows.png)

It features 12 pads and 100 high-quality drum samples.

DrumPads also works well with touchscreens, allowing you to play using multi-touch to sound multiple drums at the same time.

The samples included in this virtual drum kit are:

	22 Kick Drum
	14 Snare Drum
	2 Floor Tom
	4 Low Tom
	6 Mid Tom
	4 Hi Tom
	6 Closed Hi-Hat
	6 Open Hi-Hat
	5 Ride Cymbal
	6 Crash Cymbal
	1 Hand Drum
	2 Bongo
	2 Clap
	3 Cowbell
	1 Doumbek
	3 Timpani and Kettle Drum
	2 Woodblock
	1 Gong
	10 Other percussion sounds

These samples were created by me for http://freewavesamples.com

Keyboard control is designed to let you play the drums two-handed, with QWER 
and TYUI controlling pads 1-4, ASDF and GHJK controlling pads 5-8, and ZXCV 
and BNM, controlling pads 9-12.

You can use the left and right arrows on each pad to cycle through the 
different samples available. If you find a set of pads you like, you can 
save the drum kit to be loaded later. You can also lock the pads to prevent 
accidentally switching drum sounds during a performance.

DrumPads 2 has MIDI support, and can be played with an external MIDI controller,
drum machine, or keyboard. It works well with controllers such as the Korg 
nanoPAD 2 and responds to MIDI notes 36-47, usually the lower octave on a 
5-octave keyboard/controller and typically transmitted by drum machines.

![DrumPads MIDI Settings](https://github.com/Xangis/DrumPads/blob/master/images/DrumPadsMIDISettings.png)

It also has MIDI output support. Transmitting notes is off by default because 
on many computers the default MIDI device is the MIDI mapper and it's weird to 
hear a piano note sound when you play a drum sound. It's a simple checkbox in 
the MIDI settings box, and when turned on, you can use DrumPads to play 
external MIDI devices. This means you could play an external MIDI-enabled drum 
machine using your computer keyboard, mouse, or touchscreen.

It was originally written for webOS as a paid app. It was later ported to
Windows and Linux and released on http://zetacentauri.com. It was also ported
to OSX but never released.

A prebuilt Windows installer is available in the installer directory here:

https://github.com/Xangis/DrumPads/blob/master/installer/DrumPads2Setup.exe

## Building

In order to build this program, you'll need to have SDL installed, including
SDL_image, SDL_ttf, and SDL_mixer.

It also depends on wxWidgets and was originally built with version 3.0.2.

It also requires the wxAudioControls project from my GitHub, available here:

https://github.com/Xangis/wxAudioControls

### Windows

Use Visual Studio and load the DrumPads.sln file to load the project.

You will probably have to update library paths to match where things are on your
system in order to build.

There is an InnoSetup project at installer/DrumPads2Setup.iss that you can use
to build the installer. Paths in that are hard-coded, so you'll need to update
them to match where your project files are.

There was an attempt to package this app for the Windows 10 store, but it was
never finished. See installer/package.bat for progress so far.

### OSX

On OSX you need to have g++ avaialble via the command line and can build with 
this command:

make

You may need to edit the makefile to change paths to match where the SDL 
libraries are on your system.

The file installer/OSXLibraryPaths.sh is meant to set up proper library paths
in the application bundle, while installer/package.sh is meant to package the
app for the Apple Store. This app never made it through the review process, so
there are probably some things that need to be adjusted in those files.

### Linux

On Linux, you can get prerequisites with:

sudo apt-get install libsdl2-dev libsdl2-mixer-dev

Make with this command:

make -f Makefile.linux

You may need to edit the makefile to change paths to match where the SDL 
libraries are on your system.

## Status And Known Issues

I consider this app to be feature complete and do not plan to add any new features.
However, I do still maintain it.

This app needs some work in order to build cleanly on all operating systems. If
you make any improvements I am willing to accept pull requests.
