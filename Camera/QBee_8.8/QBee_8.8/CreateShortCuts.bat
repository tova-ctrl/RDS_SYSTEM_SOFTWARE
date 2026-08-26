@echo off
setlocal

rem Get the path of the directory where the .bat file is located
set "batDir=%~dp0"

rem Define the executable name
set "appName=QBee.exe"

rem Set the path for the executable
set "appPath=%batDir%%appName%"

rem Define the shortcut names and paths
set "shortcutNameAdmin=QBee - AdminMode.lnk"
set "shortcutPathAdmin=%batDir%%shortcutNameAdmin%"

set "shortcutNameFactory=QBee - FactoryMode.lnk"
set "shortcutPathFactory=%batDir%%shortcutNameFactory%"

rem Create the AdminMode shortcut using PowerShell
powershell -command "$WScript = New-Object -ComObject WScript.Shell; $Shortcut = $WScript.CreateShortcut('%shortcutPathAdmin%'); $Shortcut.TargetPath = '%appPath%'; $Shortcut.Arguments = '\"AdminMode\"'; $Shortcut.WorkingDirectory = '%batDir%'; $Shortcut.IconLocation = '%appPath%'; $Shortcut.Save()"

rem Create the FactoryMode shortcut using PowerShell
powershell -command "$WScript = New-Object -ComObject WScript.Shell; $Shortcut = $WScript.CreateShortcut('%shortcutPathFactory%'); $Shortcut.TargetPath = '%appPath%'; $Shortcut.Arguments = '\"FactoryMode\"'; $Shortcut.WorkingDirectory = '%batDir%'; $Shortcut.IconLocation = '%appPath%'; $Shortcut.Save()"

echo Shortcuts created:
echo - "%shortcutPathAdmin%"
echo - "%shortcutPathFactory%"
pause
