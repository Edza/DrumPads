echo This program runs the desktop app packager command to create a Windows 10 store package from an existing installer.
pause
DesktopAppConverter.exe -Installer "C:\Users\Xangis\code\DrumPads\installer\DrumPads2Setup.exe" -PackageName "DrumPads" -Version 2.0.0.0 -Makeappx -Destination "C:\Users\Xangis\code\DrumPads\installer" -InstallerArguments "/VERYSILENT" -Publisher "CN=Jason Champion" -Sign
Certutil -addStore TrustedPeople DrumPads\auto-generated.cer



