# DrumPads

A set of virtual sample-based drum pads that can be played with the keyboard,
mouse, or touchscreen. Works on Windows and Linux.

![DrumPads Screenshot](https://github.com/Xangis/DrumPads/blob/master/images/DrumPads1ScreenshotSmall.png)

It features 12 pads and 40 high-quality drum samples.

The original royalty-free samples included in this virtual drum kit are:

    6 Kick Drum
    6 Snare Drum
    2 Floor Tom
    2 Low Tom
    3 Mid Tom
    1 Hi Tom
    3 Closed Hi-Hat
    3 Open Hi-Hat
    3 Ride Cymbal
    3 Crash Cymbal
    1 Hand Drum
    2 Bongo
    2 Clap
    2 Cowbell
    1 Doumbek

These samples were created by me for http://freewavesamples.com

Keyboard control is designed to let you play the drums two-handed, with QWER 
and TYUI controlling pads 1-4, ASDF and GHJK controlling pads 5-8, and ZXCV 
and BNM, controlling pads 9-12.

DrumPads also works well with touchscreens, allowing you to play using 
multi-touch to sound multiple drums at the same time.

It was originally written for webOS as a paid app. It was later ported to
Windows and Linux and released on http://zetacentauri.com

A prebuilt Windows installer is available in the installer directory.

## Building

In order to build this program, you'll need to have SDL installed, including
SDL_image, SDL_ttf, and SDL_mixer.

On Linux, you can get prerequisites with:

sudo apt-get install libsdl2-dev libsdl2-mixer-dev
