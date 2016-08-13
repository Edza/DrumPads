#ifndef _WXDRUMPAD_H_
#define _WXDRUMPAD_H_

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include "DrumCallback.h"

class wxDrumPad : public wxControl
{
private:

    wxString _text;
    bool _arrowEnabled;
    int _width;
    int _height;
    wxBitmap* _bitmap; // background
    wxBitmap* _arrowBitmap; // Arrow
    int _midiNote; // Parent's job to translate MIDI note into pad number or whatever.
    bool _triggered; // Optional. In case we want to indicate it was triggered.
    DrumCallback* _parent;
    void OnPaint( wxPaintEvent &event );
    void NoteOn( );
    void NoteOff( );
    wxSize DoGetBestSize() const;
    DECLARE_EVENT_TABLE()

public:

    wxDrumPad();
    wxDrumPad(wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* arrowBitmap, int baseNote, DrumCallback* callback,
        wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxNO_BORDER );
    void Create( wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* arrowBitmap, int baseNote, DrumCallback* parentDlg,
        wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxNO_BORDER );
    void OnClick( wxMouseEvent &event );
    void OnRelease( wxMouseEvent &event );
    void OnRightClick( wxMouseEvent& event );
    void SetText( wxString& text );
    void OnKeyDown( wxKeyEvent& event );	
	void SetLock(bool locked);
};

#endif
