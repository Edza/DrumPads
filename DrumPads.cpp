#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#else
#include "Windows.h"
#endif
#include <math.h>
#include "DrumPads.h"
#include "../wxAudioControls/wxMidiSettingsDlg.h"
#include "wx/dir.h"
#include "wx/filename.h"

// The MIDI note to start with (for the first pad)
#define MIDI_OFFSET 36

IMPLEMENT_DYNAMIC_CLASS( DrumPads, wxDialog )

BEGIN_EVENT_TABLE( DrumPads, wxDialog )
    EVT_CLOSE( DrumPads::OnCloseWindow )
    EVT_KEY_DOWN( DrumPads::OnKeyDown )
    EVT_KEY_UP( DrumPads::OnKeyUp )
    EVT_LEFT_UP(DrumPads::OnMouseRelease) // Catches note off outside of an octave control    
	EVT_BUTTON( ID_MIDIBUTTON, DrumPads::OnMidiSettings )
	EVT_BUTTON( ID_SAVEBUTTON, DrumPads::OnSave )
	EVT_BUTTON( ID_LOADBUTTON, DrumPads::OnLoad )
	EVT_BUTTON( ID_LOCKBUTTON, DrumPads::OnLock )
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
	// TODO: Send outgoing MIDI.
	int realNote = note - MIDI_OFFSET;
	if( realNote < 0 || realNote > NUM_PADS )
	{
		// Invalid note.
		//wxMessageBox("Invalid note.");
	}
	else
	{
		Mix_PlayChannel(-1, _sample[realNote], 0);
	}
}

/**
* Enables or disables MIDI output.
*/
void DrumPads::EnableMidiOutput(bool enabled)
{
	_midiOutputEnabled = enabled;
}

void DrumPads::StopNote( int note, bool receivedFromMidi )
{
	// TODO: Send outgoing MIDI.
	int realNote = note - MIDI_OFFSET;
	if( realNote < 0 || realNote > NUM_PADS )
	{
		// Invalid note.
		//wxMessageBox("Invalid note.");
	}
	else
	{
		// TODO: Stop the currently playing SDL mix  channel if possible.
		//Mix_PlayChannel(-1, _sample[realNote], 0);
	}
}

void DrumPads::ArrowClicked( int note )
{
	note = note - MIDI_OFFSET;
	if( note < 0 || note > NUM_PADS )
	{
		// Invalid note.
		//wxMessageBox("Invalid note.");
		return;
	}
	// TODO: Advance to the next sample, set the filename and number, load the file, and replace the current _sample data with the new one.
	 _sampleSetting[note] = (_sampleSetting[note] + 1) % _waveFileNames.Count();
	 Mix_FreeChunk(_sample[note]);
	_sample[note] = Mix_LoadWAV(_waveFileNames[_sampleSetting[note]].mb_str().data());
	wxString title = wxFileName(_waveFileNames[_sampleSetting[note]]).GetName();
	_pads[note]->SetText(title);
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
	InitializeMidi();

    return true;
}
 
bool DrumPads::CreateControls()
{
	_locked = false;
	_textColour = *wxWHITE;
	_backgroundColour.Set(0, 41, 102);

	wxBoxSizer* vertsizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(vertsizer);

	wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	vertsizer->Add(buttonsizer);

	_loadButton = new wxButton(this, ID_LOADBUTTON, "Load");
	buttonsizer->Add(_loadButton);

	_saveButton = new wxButton(this, ID_SAVEBUTTON, "Save");
	buttonsizer->Add(_saveButton);

	_midiButton = new wxButton(this, ID_MIDIBUTTON, "MIDI");
	buttonsizer->Add(_midiButton);

	_lockButton = new wxButton(this, ID_LOCKBUTTON, "Lock");
	buttonsizer->Add(_lockButton);

    wxGridSizer* sizer = new wxGridSizer(NUM_PADS_HIGH, NUM_PADS_WIDE, 0, 0);
	vertsizer->Add(sizer);

    // Load images.
	_padImage = new wxBitmap();
	_padImage->LoadFile(_("button256blue.png"), wxBITMAP_TYPE_PNG);
	_arrowImage = new wxBitmap();
    _arrowImage->LoadFile(_("arrowblue.png"), wxBITMAP_TYPE_PNG);
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
        _pads[i] = new wxDrumPad(this, title, _padImage, _arrowImage, MIDI_OFFSET+i, this, 500+i, wxPoint(0,0), wxSize(256,256));
        sizer->Add(_pads[i], 0, i % NUM_PADS_WIDE, i / NUM_PADS_WIDE);
    }

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

	return true;
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
    printf("Found %d samples.\n", numFound);
	if( numFound < 1 )
	{
		wxMessageBox(wxString(_("No samples found. Cannot play audio.")), wxString(_("No Samples Found")));
	}
	else
	{
#ifdef DEMO
		_sampleSetting[0] = 2;
		_sampleSetting[1] = 14;
		_sampleSetting[2] = 22;
		_sampleSetting[3] = 26;
#else
		for( int j = 0; j < NUM_PADS; j++ )
		{
			_sampleSetting[j] = j * 6 + 4;
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
			wxString title = wxFileName(_waveFileNames[_sampleSetting[i]]).GetName();
			//_pads[i]->SetText(_waveFileNames[_sampleSetting[i]]);
			_pads[i]->SetText(title);
		}
		return false;
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
	//char keychar = (char)key;
	//wxMessageBox(wxString::Format(_("%d: %c"), key, keychar));
    switch (tolower((char)key))
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
	//wxMessageBox(wxString::Format(_("Keycode: %d, Pad: %d"), key, padnumber));
    if( padnumber == -1 )
    {
        event.Skip(true);
        return;
    }
    // Always retrigger.
	PlayNote(padnumber + MIDI_OFFSET, false);
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

void DrumPads::OnSave( wxCommandEvent& )
{
}

void DrumPads::OnLoad( wxCommandEvent& )
{
}

void DrumPads::OnLock( wxCommandEvent& )
{
	_locked = !_locked;
	for( int i = 0; i < NUM_PADS; i++ )
	{
		_pads[i]->SetLock(_locked);
	}
	if( _locked )
	{
		_lockButton->SetLabel(wxString(_("Unlock")));
	}
	else
	{
		_lockButton->SetLabel(wxString(_("Lock")));
	}
}

/**
* Displays the MIDI settings dialog, changing settings based on responses.
*/
void DrumPads::OnMidiSettings( wxCommandEvent& )
{
	wxMidiSettingsDlg* dlg = new wxMidiSettingsDlg(this, this);
    dlg->SetForegroundColour(_textColour);
    dlg->SetBackgroundColour(_backgroundColour);
	dlg->SetMidiOutputCheckbox(_midiOutputEnabled);
	dlg->SetMidiInputDeviceIndex(_midiInputDeviceNumber );
	dlg->SetMidiOutputDeviceIndex(_midiOutputDeviceNumber );
	dlg->SetMidiInputChannel(_inputChannel);
	dlg->SetMidiOutputChannel(_outputChannel);
	dlg->ShowModal();
	delete dlg;
}

bool DrumPads::InitializeMidi()
{
    _midiInDevice = new RtMidiIn();
	_midiOutDevice = new RtMidiOut();
	EnableMidiOutput(false);
	_midiInputDeviceNumber = 0;  // MIDI Mapper
	_midiOutputDeviceNumber = 0;  // Default Output
    // Open default MIDI devices.
    int numDevices = _midiInDevice->getPortCount();
    if( numDevices > 0 )
    {
	    SelectMidiInputDevice(_midiInputDeviceNumber);
    }
    else
    {
	    wxMessageBox(_("No MIDI input devices detected.  MIDI input is disabled."));
    }
    numDevices = _midiOutDevice->getPortCount();
    if( numDevices > 0 )
    {
	    SelectMidiOutputDevice(_midiOutputDeviceNumber);
    }
    else
    {
	    wxMessageBox(_("No MIDI output devices detected.  MIDI output is disabled."));
    }
	return true;
}


/**
* Changes the MIDI input channel.
*/
void DrumPads::SelectMidiInputChannel( int number )
{
    // Set MIDI Channel
	if( number > 16 )
	{
		number = 1;
	}
	AllNotesOff(); // We may or may not need to turn off all notes that were triggered by MIDI.
	_inputChannel = number;
}

/**
* Changes the MIDI output channel, turning off all notes on the previous channel.
*/
void DrumPads::SelectMidiOutputChannel( int number )
{
    // Set MIDI Channel
	if( number > 16 )
	{
		number = 1;
	}
	AllNotesOff();
	_outputChannel = number;
}


/**
* Turns off all notes and indicators.
*/
void DrumPads::AllNotesOff( bool receivedFromMidi )
{
	// TODO: Track and stop all notes.
	//for( int i = 0; i < MAX_NOTES; i++ )
	//{
	//	if( _playing[i] )
	//	{
	//		StopNote(i);
	//	}
	//	_playing[i] = false;
	//}
	if( _midiOutputEnabled && !receivedFromMidi )
	{
		// 00 (not used), 0x00, 123, 0xBX (message + channel)
		SendMidiMessage( 0, 0, 123, (175 + _outputChannel) );
	}
	// Turn off any red dots.
	//Refresh();
}


void DrumPads::SendMidiMessage(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, bool shortmsg)
{
    std::vector<unsigned char> msg;
    msg.push_back(byte4);
    msg.push_back(byte3);
    if(!shortmsg)
    {
      msg.push_back(byte2);
      if( byte1 != 0 )
      {
        msg.push_back(byte1);
      }
#ifndef VST
      _midiOutDevice->sendMessage(&msg);
#endif
    }
}


/**
* Processes MIDI input device selection changes.
*/
void DrumPads::SelectMidiInputDevice(int number)
{
#ifndef VST
	_midiInputDeviceNumber = number;
    try
    {
        _midiInDevice->closePort();
        _midiInDevice->openPort(number);
        _midiInDevice->setCallback(MidiMessageHandler, this);
    }
    //catch( RtMidiError &error )
    catch( RtMidiError &error )
    {
        //wxMessageBox(wxString::FromAscii(error.what()));
        wxMessageBox(wxString::FromAscii(error.what()));
    }
#endif
}

/**
* Processes MIDI output device selection changes.
*/
void DrumPads::SelectMidiOutputDevice(int number)
{
#ifndef VST
	_midiOutputDeviceNumber = number;
    try
    {
        _midiOutDevice->closePort();
        _midiOutDevice->openPort(number);
    }
    //catch( RtMidiError &error )
    catch( RtMidiError &error )
    {
        //wxMessageBox(wxString::FromAscii(error.what()));
        wxMessageBox(wxString::FromAscii(error.what()));
    }
#endif
}

/**
* Handles incoming MIDI message data.
*/
void DrumPads::ProcessMidiMessage(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4)
{
    //std::cout << "Received MIDI Message: " << (int)byte1 << "," << (int)byte2 << "," << (int)byte3 << "," << (int)byte4 << std::endl;
	// MIDI timimg clock pulse.  Doesn't mean anything to us.
	if( byte4 == 0xF8 )
	{
		return;
	}

	// Active sensing message.  Doesn't mean anything to us.
	if( byte4 == 0xFE )
	{
		return;
	}

	// Volume message. Doesn't mean anything to us, except for volume 0, which is note off.
	if( byte4 == (0x90 + _inputChannel - 1) )
	{
		if( byte2 > 0 )
		{
			PlayNote(byte3, true);
		}
		else
		{
			StopNote(byte3, true);
		}
		return;
	}
	// Note off (some keyboards send this instead of note on 0 velocity.
	if( byte4 == (0x80 + _inputChannel - 1)  )
	{
		StopNote( byte3, true );
		return;
	}
	// Program change. Means nothing to us.
	//else if( byte4 == (0xC0 + _inputChannel - 1)  )
	//{
	//	SetPatch( byte3, true );
	//	return;
	//}
	else if( byte4 == (0xB0 + _inputChannel - 1)  )
	{
		// All notes off.
		if( byte3 == 0x7B && byte2 == 0x00 )
		{
			AllNotesOff(true);
            return;
		}
		// Mod wheel means nothing to us.
        //else if( byte3 == 0x01 )
        //{
        //    _modWheel->SetValue( byte2 * 128 );
        //    return;
        //}
	}
	// Pitch wheel means nothing to us.
    //else if( byte4 == (0xE0 + _inputChannel - 1) )
    //{
    //    // Pitch Wheel
    //    _pitchWheel->SetValue( byte2 * 128 );
    //    return;
    //}
}

/**
* Callback for MIDI message data.
*/
void MidiMessageHandler( double deltatime, std::vector< unsigned char > *message, void *userData )
{
	DrumPads* drumpads = (DrumPads*)userData;

    unsigned char a = 0;
    unsigned char b = 0;
    unsigned char c = 0;
    unsigned char d = 0;
    if( message->size() == 4 )
    {
        a = (*message)[3];
        b = (*message)[2];
        c = (*message)[1];
        d = (*message)[0];
    }
    else if( message->size() == 3 )
    {
        b = (*message)[2];
        c = (*message)[1];
        d = (*message)[0];
    }
    else if( message->size() == 2 )
    {
        c = (*message)[1];
        d = (*message)[0];
    }
    else if( message->size() == 1 )
    {
        d = (*message)[0];
    }
	drumpads->ProcessMidiMessage(a, b, c, d);
}
