; -- Pro.iss --
; This is the install script for Hack-It Pro.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING SETUP SCRIPTS!

[Setup]
AppName=Hack-It Pro
AppVerName=Hack-It v4.0 Pro
AppPublisher=Headius, Inc
AppPublisherURL=http://www.headius.com
AppSupportURL=http://www.headius.com/support.html
AppUpdatesURL=http://www.headius.com/hackit.html
AppVersion=4.0
AppCopyright=Copyright © 1997-2002 Headius, Inc.
DefaultDirName={pf}\Hack-It
DefaultGroupName=Hack-It
MinVersion=4,4
LicenseFile=C:\HackIt\Install\Pro.txt
OutputDir=Pro
OutputBaseFilename=hackit4prosetup
WizardStyle=modern
WizardImageFile=wizard_image.bmp
WizardSmallImageFile=wizard_image_small.bmp
UninstallStyle=modern

[Files]
Source: "C:\HackIt\Pro\HackIt.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "C:\HackIt\Pro\Trayman.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "C:\HackIt\Help\HackIt.chm"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "C:\HackIt\Install\Pro.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace

[Icons]
Name: "Startup\Hack-It Pro"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
Name: "{group}\Hack-It Pro"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
Name: "{group}\Hack-It Help File"; Filename: "{app}\HackIt.chm"; WorkingDir: "{app}"
Name: "{group}\Hack-It Pro License"; Filename: "{app}\Pro.txt"; WorkingDir: "{app}"

[Registry]
Root: HKCU; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKCU; SubKey: "Software\Headius\Hack-It"; Flags: uninsdeletekey
Root: HKLM; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKLM; SubKey: "Software\Headius\Hack-It"; Flags: uninsdeletekey
