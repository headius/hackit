; -- Sample1.iss --
; Demonstrates copying 3 files, creating an icon, and some registry entries.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING SETUP SCRIPTS!

[Setup]
AppName=Hack-It
AppVerName=Hack-It v4.02
AppPublisher=Headius, Inc
AppPublisherURL=http://www.headius.com
AppSupportURL=http://www.headius.com/support.html
AppUpdatesURL=http://www.headius.com/hackit.html
AppVersion=4.02
AppCopyright=Copyright © 1997-2002 Headius, Inc.
DefaultDirName={pf}\Hack-It
DefaultGroupName=Hack-It
MinVersion=4,4
LicenseFile=..\Install\License.txt
OutputDir=Release
OutputBaseFilename=hackit4setup
WizardStyle=modern
WizardImageFile=wizard_image.bmp
WizardSmallImageFile=wizard_image_small.bmp
UninstallStyle=modern

[Files]
Source: "..\Release\HackIt.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "..\Release\Trayman.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "..\Help\HackIt.chm"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: restartreplace
Source: "..\Install\License.txt"; DestDir: "{app}"; CopyMode: alwaysoverwrite

[Icons]
Name: "{group}\Hack-It"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
Name: "{group}\Hack-It Help File"; Filename: "{app}\HackIt.chm"; WorkingDir: "{app}"
Name: "{group}\Hack-It License"; Filename: "{app}\License.txt"; WorkingDir: "{app}"
Name: "{userstartup}\Hack-It"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
