#ifndef _DRUMPADSAPP_H_
#define _DRUMPADSAPP_H_

#ifdef VST
#include "aeffect.h"
#include "aeffeditor.h"
#ifdef DLLFUNCTIONS_EXPORTS
#define DLLFUNCTIONS_API __declspec(dllexport)
#else
#define DLLFUNCTIONS_API __declspec(dllimport)
#endif
extern "C" DLLFUNCTIONS_API AEffect* VSTPluginMain (audioMasterCallback audioMaster);
#endif

#include "wx/image.h"
#include "DrumPads.h"

#ifdef VST
class DrumPadsApp: public wxApp, public AEffEditor
#else
class DrumPadsApp: public wxApp
#endif
{
    DECLARE_CLASS( DrumPadsApp )
    DECLARE_EVENT_TABLE()

public:

    DrumPadsApp();
    virtual bool OnInit();
    virtual int OnExit();

#ifdef VST
    virtual bool open(void* ptr);
    virtual void close();
    DrumPadsApp(AudioEffect* pEffect);
#endif

private:

    DrumPads* _mainWindow;
    wxWindow* _parent;
};

DECLARE_APP(DrumPadsApp)

#endif
