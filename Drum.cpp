#include <stdio.h>

#include "SDL/include/SDL.h"
#undef main
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#ifndef WIN32
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#else
#include "Windows.h"
#endif
#include <math.h>

#ifdef DEMO
#define HEIGHT 512
#define WIDTH 512
#else
#define HEIGHT 768
#define WIDTH 1024
#endif

//SDL_Surface* Surface;               // Screen surface to retrieve width/height information
SDL_Renderer *renderer;
SDL_Surface* bg;
int _sampleRate = 44100;
int _sampleBlockSize = 1024;
SDL_Surface* _padImage;
SDL_Texture* _pad;
TTF_Font* font;

#ifdef DEMO
#define NUM_WAVEFORMS 30
#define NUM_PADS 4
#define NUM_PADS_WIDE 2
#define NUM_PADS_HIGH 2
const char* _waveFileNames[] = 
{
"samples//Clap-1.wav",
"samples//Clap-2.wav",
"samples//Closed-Hi-Hat-1.wav", // 2
"samples//Closed-Hi-Hat-2.wav",
"samples//Cowbell-1.wav",
"samples//Cowbell-2.wav",
"samples//Crash-Cymbal-1.wav",
"samples//Crash-Cymbal-2.wav",
"samples//Doumbek-Tek.wav",
"samples//Floor-Tom-1.wav",
"samples//Floor-Tom-2.wav",
"samples//Hand-Drum.wav",
"samples//Hi-Bongo.wav",
"samples//Hi-Tom-1.wav",
"samples//Kick-Drum-1.wav", // 14
"samples//Kick-Drum-2.wav",
"samples//Kick-Drum-3.wav",
"samples//Low-Bongo.wav",
"samples//Low-Tom-1.wav",
"samples//Low-Tom-2.wav",
"samples//Mid-Tom-1.wav",
"samples//Mid-Tom-2.wav",
"samples//Open-Hi-Hat-1.wav", // 22
"samples//Open-Hi-Hat-2.wav",
"samples//Ride-Cymbal-1.wav",
"samples//Ride-Cymbal-2.wav",
"samples//Snare-Drum-1.wav", // 26
"samples//Snare-Drum-2.wav",
"samples//Snare-Drum-3.wav",
"samples//Snare-Drum-4.wav"
};
#else
#define NUM_WAVEFORMS 40
#define NUM_PADS 12
#define NUM_PADS_WIDE 4
#define NUM_PADS_HIGH 3
const char* _waveFileNames[] = 
{
"samples//Clap-1.wav",
"samples//Clap-2.wav",
"samples//Closed-Hi-Hat-1.wav",
"samples//Closed-Hi-Hat-2.wav",
"samples//Closed-Hi-Hat-3.wav",
"samples//Cowbell-1.wav",
"samples//Cowbell-2.wav",
"samples//Crash-Cymbal-1.wav",
"samples//Crash-Cymbal-2.wav",
"samples//Crash-Cymbal-3.wav",
"samples//Doumbek-Tek.wav",
"samples//Floor-Tom-1.wav",
"samples//Floor-Tom-2.wav",
"samples//Hand-Drum.wav",
"samples//Hi-Bongo.wav",
"samples//Hi-Tom-1.wav",
"samples//Kick-Drum-1.wav",
"samples//Kick-Drum-2.wav",
"samples//Kick-Drum-3.wav",
"samples//Kick-Drum-4.wav",
"samples//Kick-Drum-5.wav",
"samples//Kick-Drum-6.wav",
"samples//Low-Bongo.wav",
"samples//Low-Tom-1.wav",
"samples//Low-Tom-2.wav",
"samples//Mid-Tom-1.wav",
"samples//Mid-Tom-2.wav",
"samples//Mid-Tom-3.wav",
"samples//Open-Hi-Hat-1.wav",
"samples//Open-Hi-Hat-2.wav",
"samples//Open-Hi-Hat-3.wav",
"samples//Ride-Cymbal-1.wav",
"samples//Ride-Cymbal-2.wav",
"samples//Ride-Cymbal-3.wav",
"samples//Snare-Drum-1.wav",
"samples//Snare-Drum-2.wav",
"samples//Snare-Drum-3.wav",
"samples//Snare-Drum-4.wav",
"samples//Snare-Drum-5.wav",
"samples//Snare-Drum-6.wav"
};
#endif 
Mix_Chunk* _sample[NUM_WAVEFORMS];
unsigned int _sampleSetting[NUM_PADS];

void AudioCallback(void* userdata, Uint8* stream, int len);

// Initializes the application data
int Init(void) 
{
	IMG_Init(IMG_INIT_PNG);
    memset(_sampleSetting, 0, sizeof(unsigned int) * NUM_PADS);
    memset(_sample, 0, sizeof(Mix_Chunk*) * NUM_WAVEFORMS);
    // Load drum pad graphic.
#ifndef WIN32
	_padImage = IMG_Load("images//button256witharrowblue.png");
#else
	_padImage = IMG_Load(".\\button256witharrowblue.png");
#endif

    if( _padImage == NULL )
    {
        printf("Failed to open button256witharrowblue.png\n");
		printf(IMG_GetError());
		//exit(1);
    }
	else
	{
		SDL_Texture* _pad = SDL_CreateTextureFromSurface(renderer, _padImage);
		if( _pad != NULL )
		{
			printf("Pad image loaded OK. Texture created.\n");
		}
		else
		{
			printf("Pad image texture creation failed.\n");
			//exit(3);
		}
	}

    // Set up the audio stream
    int result = Mix_OpenAudio(_sampleRate, AUDIO_S16SYS, 2, 512);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }

#ifdef DEMO
	_sampleSetting[0] = 2;
	_sampleSetting[1] = 14;
	_sampleSetting[2] = 22;
	_sampleSetting[3] = 26;
#else
    for( int j = 0; j < NUM_PADS; j++ )
    {
        _sampleSetting[j] = j * 3;
    }
#endif

    // Load waveforms
    for( int i = 0; i < NUM_WAVEFORMS; i++ )
    {
        _sample[i] = Mix_LoadWAV(_waveFileNames[i]);
        if( _sample[i] == NULL )
        {
            fprintf(stderr, "Unable to load wave file: %s\n", _waveFileNames[i]);
        }
    }

    // Set up font.
    TTF_Init();
#ifndef WIN32
	font = TTF_OpenFont("//usr//share//fonts//Prelude-Medium.ttf", 18);
#endif
	font = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 18);
    if( font == NULL )
    {
        fprintf(stderr, "Unable to load font /usr/share/fonts/Prelude-Medium.ttf\n");
        // Fix for testing on my Linux machine.
#ifndef WIN32
        font = TTF_OpenFont("//usr/share//fonts//truetype//msttcorefonts//Arial.ttf", 18);
#endif
        if( font == NULL )
        {
           fprintf(stderr, "Unable to load font /usr/share/fonts/truetype/msttcorefonts/Arial.ttf\n");
        }
    }

    return true;
}

// Audio processing function.
void AudioCallback(void* userdata, Uint8* stream, int len)
{
    unsigned int count;
    short value;
    double tmpval;
    short* buffer = (short *)stream;
    // This requires that all samples in a single buffer pass have the same volume.
    // If we want realtime volume, we will have to move this calculation into the loop
    // at "Put the value in the buffer".  Won't really matter because filling the buffer
    // is faster than realtime.

    for(int i = 0; i < (len/2); i+=2 )
    {
        // Calculate sample data.
        //tmpval = ((_waveTable->_waveformTable[_selectedWaveform][(int)(_phase * TABLESIZE)] * (1.0 - _vibratoDepth)) +
        //    (_waveTable->_waveformTable[_modulationWaveform][(int)(_vibratoPhase * TABLESIZE)] * _vibratoDepth ));

        // Put value in the buffer.
        //buffer[i] = (short)(tmpval * volumeModifier);
        buffer[i] = 0;

    }
}

// Main-loop workhorse function for displaying the object
void Display(void)
{
	SDL_RenderClear(renderer);
    SDL_Rect rect;
    SDL_Surface* textSurface;
    SDL_Color foregroundColor = { 192, 204, 255 };
    SDL_Color backgroundColor = { 0, 41, 102 };
    for( int i = 0; i < NUM_PADS_WIDE; i++ )
    {
        for( int j = 0; j < NUM_PADS_HIGH; j++ )
        {
            rect.x = i * 256;
            rect.y = j * 256;
            rect.w = 256;
            rect.h = 256;
			if( _pad == NULL )
			{
				//exit(2);
			}
            SDL_RenderCopy(renderer, _pad, NULL, &rect);
            textSurface = TTF_RenderText_Shaded(font, &(_waveFileNames[_sampleSetting[(j * NUM_PADS_WIDE + i)]][9]), foregroundColor, backgroundColor);
            rect.x += 16;
            rect.y += 16;
			rect.w = textSurface->w;
			rect.h = textSurface->h;
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_RenderCopy(renderer, textTexture, NULL, &rect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }
	SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
#ifndef linux
#ifndef _DEBUG
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);
#endif
#endif

    // Initialize the SDL library with the Video subsystem
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO );
    atexit(SDL_Quit);

	int width  = 640;
	int height = 480;

	// Probably don't want a resizable window...
	SDL_Window* window = SDL_CreateWindow("DrumPads",
										  SDL_WINDOWPOS_UNDEFINED,
										  SDL_WINDOWPOS_UNDEFINED,
										  WIDTH,
										  HEIGHT,
										  SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// TODO: Reimplement set icon function.
	//SDL_WM_SetIcon(SDL_LoadBMP("DrumPads.bmp"), NULL);

    // Application specific Initialize of data structures & GL states
    if (Init() == false)
        return -1;

    // Event descriptor
    SDL_Event Event;
    bool paused = false;

    bool done = false;
    while (!done)
    {
        // Render our scene
        Display();

        bool gotEvent;
        if (paused)
        {
            SDL_WaitEvent(&Event);
            gotEvent = true;
        }
        else 
        {
            gotEvent = SDL_PollEvent(&Event);
        }

        while (!done && gotEvent) 
        {
            switch (Event.type) 
            {
                case SDL_MOUSEBUTTONDOWN:
                    {
                        int pad = ( Event.button.x / 256) + ((Event.button.y / 256) * NUM_PADS_WIDE);
#ifndef WIN32
                        syslog(LOG_INFO, "Mouse Button X: %d, Y: %d, Pad: %d", Event.button.x, Event.button.y, pad);
#endif
                        if( (Event.button.x % 256 > 191) && (Event.button.y % 256 < 64))
                        {
#ifndef WIN32
                            syslog(LOG_INFO, "Next sample clicked.");
#endif
                            _sampleSetting[pad] = (_sampleSetting[pad] + 1) % NUM_WAVEFORMS;
                        }
                        else
                        {
                            Mix_PlayChannel(-1, _sample[_sampleSetting[pad]], 0);
                        }
                    }
                    break;
                // TODO: Allow playing drums via the bluetooth keyboard.
                case SDL_KEYDOWN:
                    switch (Event.key.keysym.sym) 
                    {
                        case 'q':
                        case 't':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[0]], 0);
                            break;
                        case 'w':
                        case 'y':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[1]], 0);
                            break;
                        case 'e':
                        case 'u':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[2]], 0);
                            break;
                        case 'r':
                        case 'i':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[3]], 0);
                            break;
#ifndef DEMO
                        case 'a':
                        case 'g':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[4]], 0);
                            break;
                        case 's':
                        case 'h':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[5]], 0);
                            break;
                        case 'd':
                        case 'j':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[6]], 0);
                            break;
                        case 'f':
                        case 'k':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[7]], 0);
                            break;
                        case 'z':
                        case 'b':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[8]], 0);
                            break;
                        case 'x':
                        case 'n':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[9]], 0);
                            break;
                        case 'c':
                        case 'm':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[10]], 0);
                            break;
                        case 'v':
                        case ',':
                            Mix_PlayChannel(-1, _sample[_sampleSetting[11]], 0);
                            break;
#endif
                        default:
                            break;
                    }
                    break;

				// TODO: Figure out what pause logic to use.
                case SDL_WINDOWEVENT:
					if( Event.window.event == SDL_WINDOWEVENT_SHOWN )
					{
                        //paused = !Event.active.gain;
					}
                    break;

                case SDL_QUIT:
                    // We exit anytime we get a request to quit the app
                    // all shutdown code is registered via atexit() so this is clean.
                    done = true;
                    break;

                default:
                    break;
            }
            if( !done ) gotEvent = SDL_PollEvent(&Event);
        }
#ifndef WIN32
		usleep(3000);
#else
		Sleep(3);
#endif
    }

    for( int i = 0; i < NUM_WAVEFORMS; i++ )
    {
        Mix_FreeChunk(_sample[i]);
    }

    Mix_CloseAudio();
	IMG_Quit();
    SDL_Quit();
    return 0;
}
