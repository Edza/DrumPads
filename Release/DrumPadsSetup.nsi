;Include Modern UI

  !include "MUI2.nsh"
  !include "FileAssociation.nsh"

Name "DrumPads 1.01"
OutFile "DrumPads1.01Setup.exe"
InstallDir "$PROGRAMFILES\Zeta Centauri\DrumPads"
InstallDirRegKey HKLM "Software\DrumPads" "Install_Dir"
RequestExecutionLevel admin
!define MUI_ICON "DrumPads.ico"
!define MUI_UNICON "DrumPads.ico"

;Version Information

  VIProductVersion "1.0.1.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "DrumPads"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Zeta Centauri"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2011-2013 Zeta Centauri"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "DrumPads Installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "1.0.1.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "1.0.1.0"

;Interface Settings

  !define MUI_ABORTWARNING

;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
;  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
      !define MUI_FINISHPAGE_NOAUTOCLOSE
      !define MUI_FINISHPAGE_RUN
      !define MUI_FINISHPAGE_RUN_CHECKED
      !define MUI_FINISHPAGE_RUN_TEXT "Launch DrumPads"
      !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchProgram"
      !define MUI_FINISHPAGE_SHOWREADME ""
      !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
      !define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
      !define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages
 
  !insertmacro MUI_LANGUAGE "English"

; The stuff to install
Section "DrumPads"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "DrumPads.exe"
  File "License.txt"
  File "DrumPads.ico"
  File "libfreetype-6.dll"
  File "libpng15-15.dll"
  File "SDL.dll"
  File "SDL_image.dll"
  File "SDL_mixer.dll"
  File "SDL_ttf.dll"
  File "zlib1.dll"
  File "DrumPads.bmp"
  File "button256witharrowblue.png"
  File /r "samples"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\DrumPads "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "DisplayName" "DrumPads"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "DisplayVersion" "1.01"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "Publisher" "Zeta Centauri"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "DisplayIcon" "$INSTDIR\DrumPads.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads" "NoRepair" 1
  WriteUninstaller "uninstall.exe"


SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Zeta Centauri\DrumPads"
  CreateShortCut "$SMPROGRAMS\Zeta Centauri\DrumPads\DrumPads.lnk" "$INSTDIR\DrumPads.exe" "" "" 0
  ;CreateShortCut "$SMPROGRAMS\Zeta Centauri\DrumPads\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  WriteINIStr "$SMPROGRAMS\Zeta Centauri\DrumPads\DrumPads Website.url" "InternetShortcut" "URL" "http://zetacentauri.com/software_drumpads.htm"
  
SectionEnd

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumPads"
  DeleteRegKey HKLM SOFTWARE\DrumPads

  Delete $INSTDIR\DrumPads.exe
  Delete $INSTDIR\License.txt
  Delete $INSTDIR\DrumPads.ico
  Delete $INSTDIR\libfreetype-6.dll
  Delete $INSTDIR\libpng15-15.dll
  Delete $INSTDIR\SDL.dll
  Delete $INSTDIR\SDL_image.dll
  Delete $INSTDIR\SDL_mixer.dll
  Delete $INSTDIR\SDL_ttf.dll
  Delete $INSTDIR\zlib1.dll
  Delete $INSTDIR\DrumPads.bmp
  Delete $INSTDIR\button256witharrowblue.pn
  Delete $INSTDIR\samples\*
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Zeta Centauri\DrumPads\*.*"
  Delete "$DESKTOP\DrumPads.lnk"
  Delete "$SMPROGRAMS\Zeta Centauri\DrumPads\DrumPads Website.url"
  ;DeleteINISec "$SMPROGRAMS\Zeta Centauri\DrumPads\DrumPads Website.url" "InternetShortcut"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Zeta Centauri\DrumPads\samples"
  RMDir "$SMPROGRAMS\Zeta Centauri\DrumPads"
  RMDir "$SMPROGRAMS\Zeta Centauri"
  RMDir "$INSTDIR"


SectionEnd

; This function is automatically executed when the installer finishes
Function .onGUIEnd
	; Un-register the offer manager plugin with the operating system
	UnRegDLL $PLUGINSDIR\BunndleOfferManager.reg.dll
FunctionEnd

Function LaunchProgram
  ExecShell "" "$SMPROGRAMS\Zeta Centauri\DrumPads\DrumPads.lnk"
FunctionEnd

Function finishpageaction
  CreateShortcut "$DESKTOP\DrumPads.lnk" "$INSTDIR\DrumPads.exe"
FunctionEnd