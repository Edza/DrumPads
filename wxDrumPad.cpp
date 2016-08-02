#include "wxDrumPad.h"

BEGIN_EVENT_TABLE(wxDrumPad,wxControl)
    EVT_LEFT_DOWN(wxDrumPad::OnClick)
    EVT_LEFT_UP(wxDrumPad::OnRelease)
    EVT_PAINT(wxDrumPad::OnPaint)
    EVT_RIGHT_DOWN(wxDrumPad::OnRightClick)
END_EVENT_TABLE()

wxDrumPad::wxDrumPad( wxWindow* parent, wxWindowID id, wxBitmap bitmap, int baseNote, DrumCallback* callback,
		const wxPoint &pos, const wxSize &size, long style )
{
    Create( parent, id, bitmap, baseNote, callback, pos, size, style );
}

wxDrumPad::wxDrumPad() : wxControl()
{
    _text = wxEmptyString;
    _arrowEnabled = true;
    _width = 256;
    _height = 256;
    _midiNote = 36;
    _triggered = False;
    _parent = NULL;
}

void wxDrumPad::OnClick( wxMouseEvent& event )
{
    wxPoint location = event.GetPosition();
    // Determine whether we've clicked on the arrow and whether it matters.
    if( location < ((_width * 3) / 4) || location > ((_height * 1) / 4))
    {
        NoteOn(note);
    }
    else
    {
        if( _arrowEnabled )
        {
            _parent->ArrowClicked(_midiNote);
        }
        else
        {
            NoteOn(note);
        }
    }
    event.Skip();
    Refresh();
    return;
}

void wxDrumPad::OnOctaveRelease( wxMouseEvent& )
{
    // Do nothing -- all samples play all the way through.
    //TurnOffAll();
    // All notes off on mouse release.
    //_parent->AllNotesOff();
    return;
}

void wxDrumPad::Create (wxWindow* parent, wxString* text, wxWindowID id, wxBitmap bitmap, int baseNote, DrumCallback* callback,
                     const wxPoint &pos, const wxSize &size, long style)
{
    _text = text;
    _arrowEnabled = true;
    _width = size.w;
    _height = size.h;
    _triggered = False;
    _bitmap = bitmap;
    _midiNote = baseNote;
    _parent = callback;
    wxControl::Create (parent, id, pos, size, style);
}

void wxDrumPad::SetText( wxString* text )
{
    _text = text;
    Refresh();
}

void wxDrumPad::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    dc.DrawBitmap( _bitmap, 0, 0, true );
    dc.DrawBitmap( _arrowBitmap, ((_width * 3) / 4), ((_height) / 4), true);
    dc.DrawText(_text, 8, 8, true);
    // TODO: Draw something special on each key if a specific note is playing.
}

void wxDrumPad::NoteOn( int note )
{
    _parent->PlayNote(_baseNote);
    // Not necessary: Parent turns on indicator.
    //NoteIndicatorOn( note );
}

void wxDrumPad::NoteOff( int note )
{
    //_parent->StopNote( _baseNote + note );
    //NoteIndicatorOff( note );
}

void wxDrumPad::OnRightClick( wxMouseEvent& event )
{
    OnClick(event);
}

wxSize wxDrumPad::DoGetBestSize () const
{
	return wxSize(_width, _height);
}
