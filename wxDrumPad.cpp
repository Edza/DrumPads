#include "wxDrumPad.h"

BEGIN_EVENT_TABLE(wxDrumPad,wxControl)
    EVT_LEFT_DOWN(wxDrumPad::OnClick)
    EVT_LEFT_UP(wxDrumPad::OnRelease)
    EVT_PAINT(wxDrumPad::OnPaint)
    EVT_RIGHT_DOWN(wxDrumPad::OnRightClick)
    EVT_KEY_DOWN( wxDrumPad::OnKeyDown )
END_EVENT_TABLE()

wxDrumPad::wxDrumPad( wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* leftArrowBitmap, wxBitmap* rightArrowBitmap, int baseNote, DrumCallback* callback, wxWindowID id, 
		const wxPoint &pos, const wxSize &size, long style )
{
    Create( parent, text, bitmap, leftArrowBitmap, rightArrowBitmap, baseNote, callback, id, pos, size, style );
}

wxDrumPad::wxDrumPad() : wxControl()
{
    _text = wxEmptyString;
	_bitmap = NULL;
	_leftArrowBitmap = NULL;
	_rightArrowBitmap = NULL;
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
    // Determine whether we've clicked on the arrows and whether it matters.
	if( _arrowEnabled && location.x < (_width / 4) && location.y < (_height / 4) )
	{
        _parent->ArrowClicked(_midiNote, true);
	}
	else if( _arrowEnabled && location.x > (_width * 3) / 4 && location.y < (_height / 4) )
	{
        _parent->ArrowClicked(_midiNote, false);
	}
	else
    {
        NoteOn();
	}
    event.Skip();
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

void wxDrumPad::Create (wxWindow* parent, wxString& text, wxBitmap* bitmap, wxBitmap* leftArrowBitmap, wxBitmap* rightArrowBitmap, int baseNote, DrumCallback* callback,
                     wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
    _text = text;
    _arrowEnabled = true;
    _width = size.GetWidth();
    _height = size.GetHeight();
    _triggered = false;
    _bitmap = bitmap;
    _leftArrowBitmap = leftArrowBitmap;
    _rightArrowBitmap = rightArrowBitmap;
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
    if( _arrowEnabled && _leftArrowBitmap->IsOk() )
    {
        dc.DrawBitmap( *_leftArrowBitmap, ((_width * 3) / 4), 0, true);
    }
    if( _arrowEnabled && _rightArrowBitmap->IsOk() )
    {
        dc.DrawBitmap( *_rightArrowBitmap, 0, 0, true);
    }
	wxFont currFont = this->GetFont();
	currFont.MakeBold();
	dc.SetFont(currFont);
	dc.SetTextForeground(*wxWHITE);
    dc.DrawText(_text, 20, 116);
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

void wxDrumPad::SetLock(bool locked)
{
	_arrowEnabled = !locked;
	Refresh();
}