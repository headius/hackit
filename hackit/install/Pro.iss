; -- Pro.iss --
; This is the install script for Hack-It Pro.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING SETUP SCRIPTS!

[Setup]
AppName=Hack-It Pro
AppVerName=Hack-It v4.0 RC2 Pro
AppPublisher=Headius, Inc
AppPublisherURL=http://www.headius.com
AppSupportURL=http://www.headius.com/hackit.html
AppUpdatesURL=http://www.headius.com/hackit.html
AppVersion=4.0RC2
AppCopyright=Copyright © 1997-2001 Headius, Inc.
DefaultDirName={pf}\Hack-It
DefaultGroupName=Startup
MinVersion=4,4
LicenseFile=C:\HackIt\Install\Pro.txt
OutputDir=Pro
AlwaysRestart=Yes

[Files]
Source: "C:\HackIt\Pro\HackIt.exe"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Pro\Trayman.dll"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Help\HackIt.chm"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Install\Pro.txt"; DestDir: "{app}"; Flags: restartreplace

[Icons]
Name: "{group}\Hack-It"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"

[Registry]
Root: HKCU; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKCU; SubKey: "Software\Headius\Hack-It"; Flags: uninsdeletekey
Root: HKLM; SubKey: "Software\Headius"; Flags: uninsdeletekeyifempty
Root: HKLM; SubKey: "Software\Headius\Hack-It"; Flags: uninsdeletekey
