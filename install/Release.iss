; -- Sample1.iss --
; Demonstrates copying 3 files, creating an icon, and some registry entries.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING SETUP SCRIPTS!

[Setup]
AppName=Hack-It
AppVerName=Hack-It v4.0 RC2
AppPublisher=Headius, Inc
AppPublisherURL=http://www.headius.com
AppSupportURL=http://www.headius.com/hackit.html
AppUpdatesURL=http://www.headius.com/hackit.html
AppVersion=4.0RC2
AppCopyright=Copyright © 1997-2001 Headius, Inc.
DefaultDirName={pf}\Hack-It
DefaultGroupName=Startup
MinVersion=4,4
LicenseFile=C:\HackIt\Install\License.txt
OutputDir=Release
AlwaysRestart=Yes

[Files]
Source: "C:\HackIt\Release\HackIt.exe"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Release\Trayman.dll"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Help\HackIt.chm"; DestDir: "{app}"; Flags: restartreplace
Source: "C:\HackIt\Install\License.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\Hack-It"; Filename: "{app}\HackIt.exe"; WorkingDir: "{app}"
