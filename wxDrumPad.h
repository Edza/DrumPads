#ifndef _WXDRUMPAD_H_
#define _WXDRUMPAD_H_

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include DrumCallback* _parent;

class wxDrumPad : public wxControl
{
private:

    wxString _text;
    wxString _arrowEnabled;
    wxString _width;
    wxString _height;
    wxBitmap _bitmap; // background
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
    wxDrumPad(wxWindow* parent, wxString* text, wxWindowID id, wxBitmap bitmap, int baseNote, DrumCallback* callback,
        const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxNO_BORDER );
    void Create( wxWindow* parent, wxString* text, wxWindowID id, wxBitmap bitmap, int baseNote, OctaveCallback* parentDlg,
        const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxNO_BORDER );
    void OnClick( wxMouseEvent &event );
    void OnRelease( wxMouseEvent &event );
    void OnRightClick( wxMouseEvent& event );
    void EnableArrow( bool enabled );
    void SetText( wxString* text );

}

#endif
