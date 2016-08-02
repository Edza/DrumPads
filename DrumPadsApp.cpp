#include "wx/wx.h"

#include "DrumPadsApp.h"

#ifdef VST
IMPLEMENT_APP_NO_MAIN( DrumPadsApp )
#else
IMPLEMENT_APP( DrumPadsApp )
#endif

IMPLEMENT_CLASS( DrumPadsApp, wxApp )

BEGIN_EVENT_TABLE( DrumPadsApp, wxApp )
END_EVENT_TABLE()

DrumPadsApp::DrumPadsApp()
{
    _mainWindow = NULL;
}

bool DrumPadsApp::OnInit()
{
#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif

#ifndef VST
    _mainWindow = new DrumPads(NULL, ID_DRUMPADS_DLG );
    _mainWindow->Show(true);
#else
    _mainWindow = new DrumPads(NULL, NULL, ID_DRUMPADS_DLG );
#endif

    return true;
}

int DrumPadsApp::OnExit()
{
    return wxApp::OnExit();
}

#ifdef VST
DrumPadsApp::DrumPadsApp (AudioEffect *pEffect) : AEffEditor (pEffect)
{
    _mainWindow = new wxKeyboard(NULL, NULL, ID_DRUMPADS_DLG );
    //rect.left   = 0;
    //rect.top    = 0;
    pEffect->setEditor (this);
    systemWindow = 0;
}

/**
* DLL Stuff for VST
*/

//------------------------------------------------------------------------
/** Must be implemented externally. */
extern AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

extern "C" {

#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
	#define VST_EXPORT	__attribute__ ((visibility ("default")))
#else
	#define VST_EXPORT
#endif

//------------------------------------------------------------------------
/** Prototype of the export function main */
//------------------------------------------------------------------------
VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback audioMaster)
{
    if(NULL == wxTheApp)
    {
        ::wxInitialize();
    }

	// Get VST Version of the Host
	if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
		return 0;  // old version

	// Create the AudioEffect
	AudioEffect* effect = createEffectInstance (audioMaster);
	if (!effect)
		return 0;

	// Return the VST AEffect structur
	return effect->getAeffect ();
}

// support for old hosts not looking for VSTPluginMain
#if (TARGET_API_MAC_CARBON && __ppc__)
VST_EXPORT int main_macho (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
#elif WIN32
VST_EXPORT int MAIN (audioMasterCallback audioMaster) { return (int)VSTPluginMain (audioMaster); }
#elif BEOS
VST_EXPORT int main_plugin (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
#endif

} // extern "C"

bool DrumPadsApp::open(void *ptr)
{
    // add this to update the value the first time
    //postUpdate ();

    // create a parent wxWindow from the host provided window
    _parent = new wxWindow();
    _parent->SetEventHandler(_parent);
    _parent->SetHWND((HWND) ptr);
    _parent->SetId(wxGetWindowId((HWND) ptr)); 

    //systemWindow = ptr;
    if( _mainWindow == NULL )
    {
        _mainWindow = new wxKeyboard(NULL, NULL, ID_DRUMPADS_DLG );
    }
    _mainWindow->Show(true);
    //_mainWindow->SetEventHandler(_mainWindow);
    //_mainWindow->SetHWND((HWND)ptr);
    //_mainWindow->SetId(wxGetWindowId((HWND)ptr));
    return AEffEditor::open(ptr);
}

void DrumPadsApp::close()
{
    _parent->SetHWND(NULL);
    delete _parent;
    _parent = NULL;
    //_mainWindow = NULL;

    AEffEditor::close();

    //_mainWindow->Show(false);
    //systemWindow = 0;
}
#endif
