; The name of the installer
Name "XMLTools"

; The file to write
OutFile "xmltools.exe"

; install dir
InstallDir $PROGRAMFILES\Notepad++
InstallDirRegKey HKLM "Software\Notepad++" ""

DirText "Please select your Notepad++ path below:"

; detect if notepad is currently running
!define WNDCLASS "Notepad++"
Function .onInit
  FindWindow $0 "${WNDCLASS}"
  StrCmp $0 0 continueInstall
    MessageBox MB_ICONSTOP|MB_OK "Notepad++ is running. Please close it before you continue."
    Abort
  continueInstall:
FunctionEnd

;--------------------------------

; Pages
Page components
Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "XMLTools DLL (required)"
  SectionIn RO
  
  ; The default installation directory
  DetailPrint "XMLTools is installed at: $INSTDIR\plugins"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\plugins
  
  ; Put file there
  File "..\Release\XMLTools.dll"  
SectionEnd

; Optional section (can be disabled by the user)
Section "External dependances"
  DetailPrint "External librairies are installed at: $SYSDIR"

  ; Set output path to the installation directory.
  SetOutPath $SYSDIR
  
  File "ext_libs\*.dll"
SectionEnd

