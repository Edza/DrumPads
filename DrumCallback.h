#ifndef _DRUMCALLBACK_H_
#define _DRUMCALLBACK_H_

#include "wx/event.h"

class DrumCallback
{
public:
    virtual void PlayNote( int note, bool receivedFromMidi = false ) = 0;
    virtual void StopNote( int note, bool receivedFromMidi = false ) = 0;
    virtual void ArrowClicked( int note, bool back = false ) = 0;
	virtual void OnKeyDown( wxKeyEvent& event ) = 0;
    //virtual void AllNotesOff( bool receivedFromMidi = false ) = 0;
    //virtual void OnRightClick( int note ) = 0;
};

#endif

