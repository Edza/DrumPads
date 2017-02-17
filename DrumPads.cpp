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
#include "../wxAudioControls/wxSettingsFile.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

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
    _midiInDevice = NULL;
    _midiOutDevice = NULL;
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
	if( realNote < 0 || realNote >= NUM_PADS )
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

void DrumPads::ArrowClicked( int note, bool back )
{
	note = note - MIDI_OFFSET;
	if( note < 0 || note > NUM_PADS )
	{
		// Invalid note.
		//wxMessageBox("Invalid note.");
		return;
	}
	// Advance or decrement to the next sample, set the filename and number, load the file, and replace the current _sample data with the new one.
	if( back )
	{
		 if( _sampleSetting[note] == 0 )
		 {
			 _sampleSetting[note] = _waveFileNames.Count() - 1;
		 }
                 else
                 {
		     _sampleSetting[note] = (_sampleSetting[note] - 1);
                 }
	}
	else
	{
		 _sampleSetting[note] = (_sampleSetting[note] + 1) % _waveFileNames.Count();
	}
	 Mix_FreeChunk(_sample[note]);
#ifndef __APPLE__
	wxString fileName = wxString::Format(_("samples\\%s"), _waveFileNames[_sampleSetting[note]]);
#else
	wxString fileName = wxString::Format(_("%s/samples/%s"), wxStandardPaths::Get().GetResourcesDir(), _waveFileNames[_sampleSetting[note]]);
#endif
	_sample[note] = Mix_LoadWAV(fileName.mb_str().data());
	wxString title = wxFileName(_waveFileNames[_sampleSetting[note]]).GetName();
	_pads[note]->SetText(title);
}

bool DrumPads::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    _midiInDevice = NULL;
    _midiOutDevice = NULL;
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
#ifndef __APPLE__
    wxString filepath = _("DrumPads.ico");
#else
    wxString filepath = wxString::Format(_("%s//%s"), wxStandardPaths::Get().GetResourcesDir(), _("DrumPads.ico"));
#endif
    if( _icon.LoadFile(filepath, wxBITMAP_TYPE_ICO ))
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

	_loadButton = new wxButton(this, ID_LOADBUTTON, "Load Kit");
	buttonsizer->Add(_loadButton);

	_saveButton = new wxButton(this, ID_SAVEBUTTON, "Save Kit");
	buttonsizer->Add(_saveButton);

	_midiButton = new wxButton(this, ID_MIDIBUTTON, "MIDI Settings");
	buttonsizer->Add(_midiButton);

	_lockButton = new wxButton(this, ID_LOCKBUTTON, "  Lock Pads  ");
	buttonsizer->Add(_lockButton);

    wxGridSizer* sizer = new wxGridSizer(NUM_PADS_HIGH, NUM_PADS_WIDE, 0, 0);
	vertsizer->Add(sizer);

    // Load images.
    _padImage = new wxBitmap();
#ifndef __APPLE__
    _padImage->LoadFile(_("button256blue.png"), wxBITMAP_TYPE_PNG);
#else
    wxString filepath = wxString::Format(_("%s/%s"), wxStandardPaths::Get().GetResourcesDir(), _("button256blue.png"));
    //printf("Loading %s", filepath.mb_str());
    _padImage->LoadFile(filepath, wxBITMAP_TYPE_PNG); 
#endif
    _leftArrowImage = new wxBitmap();
#ifndef __APPLE__
    _leftArrowImage->LoadFile(_("arrowblue.png"), wxBITMAP_TYPE_PNG);
#else
    filepath = wxString::Format(_("%s/%s"), wxStandardPaths::Get().GetResourcesDir(), _("arrowblue.png"));
    //printf("Loading %s", filepath.mb_str());
    _leftArrowImage->LoadFile(filepath, wxBITMAP_TYPE_PNG);
#endif
    _rightArrowImage = new wxBitmap();
#ifndef __APPLE__
    _rightArrowImage->LoadFile(_("leftarrowblue.png"), wxBITMAP_TYPE_PNG);
#else
    filepath = wxString::Format(_("%s/%s"), wxStandardPaths::Get().GetResourcesDir(), _("leftarrowblue.png"));
    //printf("Loading %s", filepath.mb_str());
    _rightArrowImage->LoadFile(filepath, wxBITMAP_TYPE_PNG);
#endif
    if( !_padImage->IsOk() )
    {
        printf("Failed to open button256blue.png\n");
    }
    if( !_leftArrowImage->IsOk() )
    {
        printf("Failed to open arrowblue.png\n");
    }
    if( !_rightArrowImage->IsOk() )
    {
        printf("Failed to open leftarrowblue.png\n");
    }

    // Create pads.
    wxString title = wxString(_("Empty"));
    for( int i = 0; i < MAX_PADS; i++ )
    {
        _pads[i] = new wxDrumPad(this, title, _padImage, _leftArrowImage, _rightArrowImage, MIDI_OFFSET+i, this, 500+i, wxPoint(0,0), wxSize(256,256));
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

    result = Mix_AllocateChannels(8);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }

    // Build wave file list.
    wxDir dir;
    wxString filename;
    int numFound = 0;
#ifdef linux
    wxString dirname = wxString(_("./samples"));
#elif __APPLE__
    wxString dirname = wxString(_(".\\samples"));
#else
    wxString dirname = wxString::Format(_("%s/samples"), wxStandardPaths::Get().GetResourcesDir());
#endif
    wxDir::GetAllFiles(dirname, &_waveFileNames, wxString(_("*.wav")), wxDIR_FILES);
    numFound = _waveFileNames.GetCount();
    printf("Found %d samples in %s.\n", numFound, dirname.mb_str().data());
	if( numFound < 1 )
	{
		wxMessageBox(wxString(_("No samples found. Cannot play audio.")), wxString(_("No Samples Found")));
	}
	else
	{
		// Fix paths so they're absolute -- they load as relative.
		for( int i = 0; i < _waveFileNames.Count(); i++ )
		{
			wxFileName fname = wxFileName(_waveFileNames[i]);
			fname.Normalize();
			// Verified working.
			//wxMessageBox(fname.GetFullPath(), _waveFileNames[i]);
			_waveFileNames[i] = fname.GetFullName();
		}
#ifdef DEMO
		_sampleSetting[0] = 2;
		_sampleSetting[1] = 14;
		_sampleSetting[2] = 22;
		_sampleSetting[3] = 26;
#else
		_sampleSetting[0] = 82;
		for( int j = 1; j < NUM_PADS; j++ )
		{
			_sampleSetting[j] = j * 7 + -3;
		}
#endif

		// Load waveforms - only the ones assigned to pads. If we change waveforms, unload and reload in that code.
		for( int i = 0; i < NUM_PADS; i++ )
		{
#ifndef __APPLE__
			wxString fileName = wxString::Format(_("samples\\%s"), _waveFileNames[_sampleSetting[i]]);
#else
	                wxString fileName = wxString::Format(_("%s/samples/%s"), wxStandardPaths::Get().GetResourcesDir(), _waveFileNames[_sampleSetting[i]]);
#endif
			_sample[i] = Mix_LoadWAV(fileName.mb_str().data());
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

	// Send the MIDI note if necessary.
	if( _midiOutputEnabled )
	{
		// 00 (not used), 7F (velocity), 2B (note number), 9X (note on)+channel
		SendMidiMessage( 0, 127, padnumber + MIDI_OFFSET, (143 + _outputChannel) );
		SendMidiMessage( 0, 127, padnumber + MIDI_OFFSET, (159 + _outputChannel) );
	}
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

void DrumPads::OnSave( wxCommandEvent& event )
{
	//wxString path = wxStandardPaths::Get().GetUserConfigDir() + _("\\..\\Local\\Sigmatizm");
	wxFileDialog fdialog( this, _("Save Config As"), _("."), _(""), _("DrumPads Kits (*.drumpads) |*.drumpads||"), wxFD_SAVE );

	wxString filename;
	
	if( fdialog.ShowModal() != wxID_OK )
	{
		return;
	}

	wxSettingsFile file;
	for( int i = 0; i < NUM_PADS; i++ )
	{
		//wxString fname = wxFileName(_waveFileNames[_sampleSetting[i]]).GetFullPath();
		wxFileName fname = wxFileName(_waveFileNames[_sampleSetting[i]]);
		fname.Normalize();
		//wxMessageBox(fname.GetFullName());
		file.SetValue(wxString::Format(_("Pad%d"), i), fname.GetFullName() );
	}
	wxString name = fdialog.GetPath();
	file.Save(name);
	event.Skip(false);
}

void DrumPads::OnLoad( wxCommandEvent& )
{
	wxFileDialog fdialog( this, _("Load A Config"), _("."), _(""), _("DrumPads Kits (*.drumpads) |*.drumpads||"), wxFD_OPEN );

	wxString filename;

	if( fdialog.ShowModal() != wxID_OK )
	{
		return;
	}

	wxString value;
	wxSettingsFile file;
	wxString fname = fdialog.GetPath();
	file.Load(fname);
	for( int i = 0; i < NUM_PADS; i++ )
	{
		bool found = false;
		value = file.GetValue(wxString::Format(_("Pad%d"), i));
		for( int j = 0; j < _waveFileNames.Count(); j++ )
		{
			if( _waveFileNames[j] == value )
			{
				_sampleSetting[i] = j;
				// DELETE EXISTING SAMPLE, LOAD NEW
				// TODO: Handle "not found" samples.
                                Mix_FreeChunk(_sample[i]);
#ifndef __APPLE__
				wxString fileName = wxString::Format(_("samples\\%s"), _waveFileNames[_sampleSetting[i]]);
#else
                                wxString fileName = wxString::Format(_("%s/samples/%s"), wxStandardPaths::Get().GetResourcesDir(), _waveFileNames[_sampleSetting[i]]);
#endif
				_sample[i] = Mix_LoadWAV(fileName.mb_str().data());
				wxString title = wxFileName(_waveFileNames[_sampleSetting[i]]).GetName();
				_pads[i]->SetText(title);
				found = true;
			}
		}
		if( !found )
		{
			wxMessageBox(value, _("Sample not found"));
		}
	}
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
		_lockButton->SetLabel(wxString(_("Unlock Pads")));
	}
	else
	{
		_lockButton->SetLabel(wxString(_("Lock Pads")));
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
    _inputChannel = 1;
    _outputChannel = 1;
    _midiInDevice = new RtMidiIn();
    _midiOutDevice = new RtMidiOut();
    EnableMidiOutput(false);
    _midiInputDeviceNumber = 0;  // MIDI Mapper
    _midiOutputDeviceNumber = 0;  // Default Output

    // Open default MIDI devices.
    int numInDevices = _midiInDevice->getPortCount();
    if( numInDevices > 0 )
    {
            SelectMidiInputDevice(_midiInputDeviceNumber);
    }
    int numOutDevices = _midiOutDevice->getPortCount();
    if( numOutDevices > 0 )
    {
            SelectMidiOutputDevice(_midiOutputDeviceNumber);
    }

    if( numOutDevices < 1 || numInDevices < 1 )
    {
        wxString in = _("");
        wxString out = _("");
        if( numInDevices < 1 )
        {
            in = _("No MIDI input devices detected.  MIDI input is disabled.");
        }
        if( numOutDevices < 1 )
        {
            out = _("No MIDI output devices detected.  MIDI output is disabled.");
        }
        wxMessageBox(wxString::Format(_("%s\n\n%s"), in, out ));
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
    catch( RtMidiError &error )
    {
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
    if( message->size() > 0 )
    {
        d = (*message)[0];
    }
    if( message->size() > 1 )
    {
        c = (*message)[1];
    }
    if( message->size() > 2 )
    {
        b = (*message)[2];
    }
    if( message->size() > 3 )
    {
        a = (*message)[3];
    }
	if( message->size() > 4 )
	{
		printf( "MIDI message size too large, cannot process." );
		return;
	}
	drumpads->ProcessMidiMessage(a, b, c, d);
}
