#include "wx/wx.h"

#include "DrumPadsApp.h"

IMPLEMENT_APP( DrumPadsApp )

IMPLEMENT_CLASS( DrumPadsApp, wxApp )

BEGIN_EVENT_TABLE( DrumPadsApp, wxApp )
END_EVENT_TABLE()

DrumPadsApp::DrumPadsApp()
{
    _mainWindow = NULL;
}

bool DrumPadsApp::OnInit()
{
    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxGIFHandler);

    _mainWindow = new DrumPads(NULL, ID_DRUMPADS_DLG );
    _mainWindow->Show(true);

    return true;
}

int DrumPadsApp::OnExit()
{
    return wxApp::OnExit();
}
