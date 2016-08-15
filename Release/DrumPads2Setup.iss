; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "DrumPads"
#define MyAppVersion "2"
#define MyAppPublisher "Zeta Centauri"
#define MyAppURL "http://zetacentauri.com/software_drumpads.htm"
#define MyAppExeName "DrumPads.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{5F81020E-409C-4E36-92BC-822B29EA3E04}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\Zeta Centauri\DrumPads
DisableDirPage=yes
DefaultGroupName=Zeta Centauri\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\Users\Xangis\code\DrumPads\Release\License.txt
OutputDir=C:\Users\Xangis\code\DrumPads\installer
OutputBaseFilename={#MyAppName}{#MyAppVersion}Setup
SetupIconFile=C:\Users\Xangis\code\DrumPads\Release\DrumPads.ico
UninstallDisplayIcon={app}\DrumPads.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\DrumPads.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\SDL2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\SDL2_mixer.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\DrumPads.bmp"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\spin.bmp"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\DrumPads.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\button256blue.png"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\arrowblue.png"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\DrumPads\Release\DrumPads\samples\*"; DestDir: "{app}\Samples"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent
