; -- Site.iss --
; This is the install script for Hack-It 50-Seat Site License.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING SETUP SCRIPTS!

[Setup]
Bits=32
AppName=Hack-It 25-Seat License
AppVerName=Hack-It v4.01 25-Seat Site License
AppCopyright=Copyright © 1997-2000 Headius, Inc.
DefaultDirName={pf}\Hack-It
DefaultGroupName=Startup
MinVersion=4,4
LicenseFile=C:\My Projects\HackIt\Site.txt
OutputDir=Site
OutputBaseFileName=SetupSL

[Files]
Source: "C:\My Projects\HackIt\Pro\HackIt.exe"; DestDir: "{app}"
Source: "C:\My Projects\HackIt\Pro\Trayman.dll"; DestDir: "{app}"
Source: "C:\My Projects\HackItHelp\HackIt.chm"; DestDir: "{app}"
Source: "C:\My Projects\HackIt\Site.txt"; DestDir: "{app}"

[Icons]
Name: "{userstartup}\Hack-It Pro"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
Name: "{group}\Hack-It Pro"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
Name: "{group}\Hack-It Help File"; Filename: "{app}\HackIt.chm"; WorkingDir: "{app}"
Name: "{group}\Hack-It Pro Site License"; Filename: "{app}\Site.txt"; WorkingDir: "{app}"
[Registry]
Root: HKCU; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKCU; SubKey: "Software\Headius\HackIt"; Flags: uninsdeletekey
Root: HKLM; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKLM; SubKey: "Software\Headius\HackIt"; Flags: uninsdeletekey
