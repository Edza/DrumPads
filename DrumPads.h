#ifndef _DRUMPADS_H_
#define _DRUMPADS_H_

//#define BUFFERLENGTH 256
//#define SAMPLE_RATE (44100)
//#define MINIPANELWIDTH 120
//#define MINIPANELHEIGHT 100

/*!
 * Includes
 */
//#ifndef VST
//#include "AudioSettingsInterface.h"
//#include "MidiSettingsInterface.h"
//#include "portaudio.h"
//#else
//#include "aeffectx.h"
//#include "audioeffectx.h"
//#endif
#include "DrumCallback.h"
#include "wxDrumPad.h"
#include "../wxAudioControls/MidiSettingsInterface.h"

#define INITGUID

#define MAX_PADS 12

/*!
 * Control identifiers
 */
#define ID_DRUMPADS_DLG 10001
#define SYMBOL_DRUMPADS_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxWANTS_CHARS|wxMINIMIZE_BOX
#define SYMBOL_DRUMPADS_TITLE _("DrumPads 2.01 by Jason Champion")
#define SYMBOL_DRUMPADS_IDNAME ID_DRUMPADS_DLG
#define SYMBOL_DRUMPADS_SIZE wxSize(1024, 768)
#define SYMBOL_DRUMPADS_POSITION wxDefaultPosition
#define ID_BANKSPIN 10001
#define ID_PATCHSPIN 10003
#define ID_DRUMPADS1 10006
#define ID_DRUMPADS2 10007
#define ID_DRUMPADS3 10008
#define ID_DRUMPADS4 10009
#define ID_DRUMPADS5 10010
#define ID_DRUMPADS6 10011
#define ID_DRUMPADS7 10012
#define ID_PATCHTEXT 10013
#define ID_CHANNELTEXT 10014
#define ID_PITCHWHEEL 10015
#define ID_MODWHEEL 10016
#define ID_PANICBUTTON 10017
#define ID_INFOBUTTON 10018
#define ID_HELPBUTTON 10019
#define ID_SAVEBUTTON 10021
#define ID_LOADBUTTON 10022
#define ID_MIDIBUTTON 10023
#define ID_BANKTEXT 10025
#define ID_VOLUME_LEFT 10027
#define ID_VOLUME_RIGHT 10028
#define ID_FILTERBUTTON 10029
#define ID_ADSRBUTTON 10030
#define ID_PARAMETERSBUTTON 10031
#define ID_LFOBUTTON 10032
#define ID_INITIALWAVE_PANEL 10033
#define ID_FINALWAVE_PANEL 10034
#define ID_BUTTON_NORMALIZE 10035
#define ID_BUTTON_INITIAL 10036
#define ID_BUTTON_FINAL 10037
#define ID_BUTTON_CLEAR 10038
#define ID_BUTTON_GENERATE 10039
#define ID_CHOICE_WAVEFORM 10040
#define ID_ZOOM_LEVEL 10041
#define ID_BUTTON_LEFT 10042
#define ID_BUTTON_RIGHT 10043
#define ID_LFO_FREQUENCY 10044
#define ID_LFO_WAVEFORM 10045
#define ID_TXT_FREQUENCY 10046
#define ID_ADSR_ATTACK 10047
#define ID_ADSR_DECAY 10048
#define ID_ADSR_SUSTAIN 10049
#define ID_ADSR_RELEASE 10050
#define ID_TXT_ATTACK 10051
#define ID_TXT_DECAY 10052
#define ID_TXT_SUSTAIN 10053
#define ID_TXT_RELEASE 10054
#define ID_BUTTON_SETTINGS 10055
#define ID_LOCKBUTTON 10056

/*!
 * Compatibility
 */
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

// Demo is restricted. Num pads and max pads are different because maybe that's configurable via settings.
#ifdef DEMO
#define NUM_WAVEFORMS 30
#define NUM_PADS 4
#define MAX_PADS 4
#define NUM_PADS_WIDE 2
#define NUM_PADS_HIGH 2
#else
#define NUM_WAVEFORMS 25000
#define NUM_PADS 12
#define MAX_PADS 12
#define NUM_PADS_WIDE 4
#define NUM_PADS_HIGH 3
#endif

#ifdef linux
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#else
// Requires libsdl2-mixer-dev
#include "SDL.h"
#include "SDL_mixer.h"
#endif

//class DrumPads: public wxDialog, public DrumCallback, public MidiSettingsInterface, public AudioSettingsInterface
class DrumPads: public wxDialog, public DrumCallback, public MidiSettingsInterface
{
    DECLARE_DYNAMIC_CLASS( DrumPads )
    DECLARE_EVENT_TABLE()
public:
    virtual ~DrumPads();
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DRUMPADS_IDNAME, const wxString& caption = SYMBOL_DRUMPADS_TITLE, const wxPoint& pos = SYMBOL_DRUMPADS_POSITION, const wxSize& size = SYMBOL_DRUMPADS_SIZE, long style = SYMBOL_DRUMPADS_STYLE );
    bool CreateControls();
    bool InitializeAudio();
	bool InitializeMidi();
    void OnCloseWindow( wxCloseEvent& event );
    void OnKeyDown( wxKeyEvent& event );
    void OnKeyUp( wxKeyEvent& event );
    void OnMouseRelease( wxMouseEvent &event );  
    DrumPads();
    DrumPads(wxWindow* parent, wxWindowID id = SYMBOL_DRUMPADS_IDNAME, const wxString& caption = SYMBOL_DRUMPADS_TITLE, const wxPoint& pos = SYMBOL_DRUMPADS_POSITION, const wxSize& size = SYMBOL_DRUMPADS_SIZE, long style = SYMBOL_DRUMPADS_STYLE );
    //void SendMidiMessage( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, bool shortmsg = false );
    // DrumCallback members.
    void PlayNote( int note, bool receivedFromMidi = false );
    void StopNote( int note, bool receivedFromMidi = false );
    int GetPadNumber(int key);
    void ArrowClicked( int note, bool back = false );
    //void StopNote( int note, bool receivedFromMidi = false );
    //void AllNotesOff( bool receivedFromMidi = false );
    //void ProcessMidiMessage(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);
	bool _done;
	// MidiSettingsInterface methods.
	void SelectMidiInputDevice(int number);
	void SelectMidiOutputDevice(int number);
	void SelectMidiInputChannel(int number);
	void SelectMidiOutputChannel(int number);
	void EnableMidiOutput(bool enabled);
	// End MidiSettingsInterface methods.
	void OnSave( wxCommandEvent& event );
	void OnLoad( wxCommandEvent& event );
	void OnLock( wxCommandEvent& event );
	void OnMidiSettings( wxCommandEvent& event );
	void AllNotesOff( bool receivedFromMidi = false );
    void SendMidiMessage( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, bool shortmsg = false );
    void ProcessMidiMessage(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);
private:
    wxDrumPad* _pads[MAX_PADS];
    Mix_Chunk* _sample[NUM_PADS];
    unsigned int _sampleSetting[NUM_PADS];
    wxArrayString _waveFileNames;
    wxBitmap* _padImage;
    wxBitmap* _leftArrowImage;
    wxBitmap* _rightArrowImage;
	wxButton* _loadButton;
	wxButton* _saveButton;
	wxButton* _midiButton;
	wxButton* _lockButton;
	bool _locked;
    wxIcon _icon;
    int _sampleRate;
    int _sampleBlockSize;
    wxColour _textColour;
    wxColour _backgroundColour;
	// MIDI Settings
    int _inputChannel;
	int _outputChannel;
	int _midiInputDeviceNumber;
	int _midiOutputDeviceNumber;
	bool _midiOutputEnabled;
	// End MIDI Settings
};

// Callback for handling incoming MIDI messages.
void MidiMessageHandler( double deltatime, std::vector< unsigned char > *message, void *userData );

#endif
