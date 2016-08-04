#include <stdio.h>

//#include "SDL/include/SDL.h"
//#undef main
#ifndef WIN32
// Requires libsdl2-mixer-dev libsdl2-image-dev libsdl2-ttf-dev
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#else
#include "Windows.h"
#endif
#include <math.h>
#include "DrumPads.h"
#include "wx/dir.h"

IMPLEMENT_DYNAMIC_CLASS( DrumPads, wxDialog )

BEGIN_EVENT_TABLE( DrumPads, wxDialog )
    EVT_CLOSE( DrumPads::OnCloseWindow )
    EVT_KEY_DOWN( DrumPads::OnKeyDown )
    EVT_KEY_UP( DrumPads::OnKeyUp )
    EVT_LEFT_UP(DrumPads::OnMouseRelease) // Catches note off outside of an octave control    
END_EVENT_TABLE()

// Initializes the application data
/*int Init(void) 
{
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
*/

DrumPads::DrumPads()
{
}

DrumPads::DrumPads(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

DrumPads::~DrumPads()
{
    for( int i = 0; i < NUM_PADS; i++ )
    {
        Mix_FreeChunk(_sample[i]);
    }

    Mix_CloseAudio();
    
    SDL_Quit();
}

void DrumPads::PlayNote( int note, bool receivedFromMidi )
{
}

void DrumPads::ArrowClicked( int note )
{
}

bool DrumPads::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
    if( _icon.LoadFile(_T("DrumPads.ico"), wxBITMAP_TYPE_ICO ))
    {
        SetIcon(_icon);
    }
    InitializeAudio();

    return true;
}
 
bool DrumPads::CreateControls()
{
    wxGridSizer* sizer = new wxGridSizer(NUM_PADS_HIGH, NUM_PADS_WIDE, 0, 0);
    this->SetSizer(sizer);

    // Load images.
#ifndef WIN32
        _padImage = new wxBitmap(wxString(_("images//button256blue.png")));
        _arrowImage = new wxBitmap(wxString(_("images//arrowblue.png")));
#else
        _padImage = new wxBitmap(wxString(_(".\\button256blue.png")));
        _arrowImage = new wxBitmap(wxString(_(".\\arrowblue.png")));
#endif
    if( _padImage->IsOk() )
    {
        printf("Loaded button256blue.png\n");
    }
    else
    {
        printf("Failed to open button256blue.png\n");
    }
    if( _arrowImage->IsOk() )
    {
        printf("Loaded arrowblue.png\n");
    }
    else
    {
        printf("Failed to open arrowblue.png\n");
    }

    // Create pads.
    wxString title = wxString(_("Empty"));
    for( int i = 0; i < MAX_PADS; i++ )
    {
        _pads[i] = new wxDrumPad(this, title, _padImage, _arrowImage, 36+i, this, 500+i, wxPoint(0,0), wxSize(256,256));
        sizer->Add(_pads[i], 0, i % NUM_PADS_WIDE, i / NUM_PADS_WIDE);
    }

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
}

bool DrumPads::InitializeAudio()
{
    _sampleRate = 44100;
    _sampleBlockSize = 1024;
    // Initialize the SDL library with the Video subsystem
    SDL_Init(SDL_INIT_AUDIO);
    atexit(SDL_Quit);

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

    // Build wave file list.
    wxDir dir;
    wxString filename;
    int numFound = 0;
    wxDir::GetAllFiles(wxString(_("./samples")), &_waveFileNames, wxString(_("*.wav")), wxDIR_FILES);
    numFound = _waveFileNames.GetCount();
    /*if( dir.Open(wxString(_("./samples"))) )
    {
        bool cont = dir.GetFirst(&filename, wxString(_("*.wav")), wxDIR_FILES);
        while(true)
        {
            wprintf(_("Found %s\n"), filename.mb_str().data());
            _waveFileNames.Add(filename);
            ++numFound;
            cont = dir.GetNext(&filename);
            if( cont >= NUM_WAVEFORMS )
            {
                break;
            }
        }
    }*/
    printf("Found %d samples.\n", numFound);

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

    // Load waveforms - only the ones assigned to pads. If we change waveforms, unload and reload in that code.
    for( int i = 0; i < NUM_PADS; i++ )
    {
        _sample[i] = Mix_LoadWAV(_waveFileNames[_sampleSetting[i]].mb_str().data());
        if( _sample[i] == NULL )
        {
            wprintf(_("Unable to load wave file: %s\n"), _waveFileNames[_sampleSetting[i]].mb_str().data());
        }
    }

    return true;
}

/**
* Handles window close events.
*/
void DrumPads::OnCloseWindow( wxCloseEvent& event )
{
    this->Destroy();
    event.Skip();
}

// Process keyboard key and get the pad number it triggers. Don't play the sample, just return the pad id.
int DrumPads::GetPadNumber(int key)
{
    switch (key)
    {
        case 'q':
        case 't':
            return 0;
            break;
        case 'w':
        case 'y':
            return 1;
            break;
        case 'e':
        case 'u':
            return 2;
            break;
        case 'r':
        case 'i':
            return 3;
            break;
#ifndef DEMO
        case 'a':
        case 'g':
            return 4;
            break;
        case 's':
        case 'h':
            return 5;
            break;
        case 'd':
        case 'j':
            return 6;
            break;
        case 'f':
        case 'k':
            return 7;
            break;
        case 'z':
        case 'b':
            return 8;
            break;
        case 'x':
        case 'n':
            return 9;
            break;
        case 'c':
        case 'm':
            return 10;
            break;
        case 'v':
        case ',':
            return 11;
            break;
#endif
        default:
            break;
    }
    return -1;
}

/**
* Handles key down events.
*/
void DrumPads::OnKeyDown( wxKeyEvent& event )
{
    int key = event.GetKeyCode();
    // Tab navigation explicitly disabled.
    if( key == WXK_TAB )
    {
        return;
    }
    int padnumber = GetPadNumber( key );
    if( padnumber == -1 )
    {
        event.Skip(true);
        return;
    }
    // Always retrigger.
    Mix_PlayChannel(-1, _sample[padnumber], 0);
    // TODO: Send the MIDI note if necessary.
    //PlayNote( note );
    event.Skip(true);
}

/**
* Handles key up events, turns off a playing note.
*/
void DrumPads::OnKeyUp( wxKeyEvent& event )
{
    /*
    // Not necessary because all samples are one-shots that play all the way through.
    int key = event.GetKeyCode();
    int note = GetNoteValue( key );
    if( note == 0 )
    {
            event.Skip(true);
            return;
    }
    StopNote( note );
    */
    event.Skip(true);
}

/**
* Handles release of the left mouse button.
*/
void DrumPads::OnMouseRelease( wxMouseEvent& event )
{
    //AllNotesOff();
    event.Skip();
}
