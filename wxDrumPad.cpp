#include "wxDrumPad.h"

BEGIN_EVENT_TABLE(wxDrumPad,wxControl)
    EVT_LEFT_DOWN(wxDrumPad::OnClick)
    EVT_LEFT_UP(wxDrumPad::OnRelease)
    EVT_PAINT(wxDrumPad::OnPaint)
    EVT_RIGHT_DOWN(wxDrumPad::OnRightClick)
    EVT_KEY_DOWN( wxDrumPad::OnKeyDown )
END_EVENT_TABLE()

wxDrumPad::wxDrumPad( wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* arrowBitmap, int baseNote, DrumCallback* callback, wxWindowID id, 
		const wxPoint &pos, const wxSize &size, long style )
{
    Create( parent, text, bitmap, arrowBitmap, baseNote, callback, id, pos, size, style );
}

wxDrumPad::wxDrumPad() : wxControl()
{
    _text = wxEmptyString;
    _arrowEnabled = true;
    _width = 256;
    _height = 256;
    _midiNote = 36;
    _triggered = false;
    _parent = NULL;
}

void wxDrumPad::OnClick( wxMouseEvent& event )
{
	//wxMessageBox(wxString(_("Note on")));
    wxPoint location = event.GetPosition();
    // Determine whether we've clicked on the arrow and whether it matters.
    if( location.x < ((_width * 3) / 4) || location.y > ((_height * 1) / 4))
    {
        NoteOn();
    }
    else
    {
        if( _arrowEnabled )
        {
            _parent->ArrowClicked(_midiNote);
        }
        else
        {
            NoteOn();
        }
    }
    event.Skip();
    Refresh();
    return;
}

void wxDrumPad::OnRelease( wxMouseEvent& )
{
    // Do nothing -- all samples play all the way through.
    //TurnOffAll();
    // All notes off on mouse release.
    //_parent->AllNotesOff();
    return;
}

void wxDrumPad::Create (wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* arrowBitmap, int baseNote, DrumCallback* callback,
                     wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
    _text = text;
    _arrowEnabled = true;
    _width = size.GetWidth();
    _height = size.GetHeight();
    _triggered = false;
    _bitmap = bitmap;
    _arrowBitmap = arrowBitmap;
    _midiNote = baseNote;
    _parent = callback;
    wxControl::Create (parent, id, pos, size, style);
}

void wxDrumPad::SetText( wxString& text )
{
    _text = text;
    Refresh();
}

void wxDrumPad::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    if( _bitmap->IsOk() )
    {
        dc.DrawBitmap( *_bitmap, 0, 0, true );
    }
    if( _arrowBitmap->IsOk() )
    {
        dc.DrawBitmap( *_arrowBitmap, ((_width * 3) / 4), 0, true);
    }
	wxFont currFont = this->GetFont();
	currFont.MakeBold();
	dc.SetFont(currFont);
	dc.SetTextForeground(*wxWHITE);
    dc.DrawText(_text, 20, 16);
    // TODO: Draw something special on each key if a specific note is playing.
}

void wxDrumPad::NoteOn()
{
    _parent->PlayNote(_midiNote);
    // Not necessary: Parent turns on indicator.
    //NoteIndicatorOn( note );
}

void wxDrumPad::NoteOff()
{
    _parent->StopNote(_midiNote);
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

void wxDrumPad::OnKeyDown( wxKeyEvent& event )
{
	//wxMessageBox("Key event");
	this->_parent->OnKeyDown(event);
}