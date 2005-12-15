; NSIS installer script for Nonpareil
; Copyright 2005 Eric L. Smith <eric@brouhaha.com>
; $Id$

; This script was originally generated by Christoph Giesselink using 
; the HM NIS Edit Script Wizard.

!define VER_MAJOR 0
!define VER_MINOR 7
!define VER_REVISION 7
!define VER_BUILD 0

!define VER_FILE    "${VER_MAJOR}${VER_MINOR}${VER_REVISION}"
!define VER_DISPLAY "${VER_MAJOR}.${VER_MINOR}${VER_REVISION}"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Nonpareil"
!define PRODUCT_VERSION ${VER_DISPLAY}
!define PRODUCT_ROOT_FOLDER "HP-Emulators"
!define PRODUCT_WEB_SITE "http://nonpareil.brouhaha.com/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "StartMenuDir"

; define GTK+ 2 installer packet to add it to the Nonpareil distribution
!define GTK2 "gtk2-runtime-2.6.8-050618-ash.exe"
;!define GTK2 "gtk2-runtime-2.8.7-2005-11-21-ash.exe"

SetCompressor /FINAL /SOLID lzma

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}v${VER_FILE}Setup.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_ROOT_FOLDER}\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
BrandingText "nonpareil.brouhaha.com"
;ShowInstDetails show
;ShowUnInstDetails show
CRCCheck force

; installation file properties
VIProductVersion "${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD}"
VIAddVersionKey "CompanyName" "Eric Smith"
VIAddVersionKey "FileDescription" "Classic/Woodstock/Spice/Coconut/Voyager Series Installation"
VIAddVersionKey "FileVersion" "${VER_MAJOR}, ${VER_MINOR}, ${VER_REVISION}, ${VER_BUILD}"
VIAddVersionKey "InternalName" "${PRODUCT_NAME}v${VER_FILE}Setup"
VIAddVersionKey "LegalCopyright" "Copyright � 2005"
VIAddVersionKey "OriginalFilename" "${PRODUCT_NAME}v${VER_FILE}Setup.exe"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}v${VER_FILE}Setup"
VIAddVersionKey "ProductVersion" "${VER_MAJOR}, ${VER_MINOR}, ${VER_REVISION}, ${VER_BUILD}"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "FileFunc.nsh"
!insertmacro un.GetParent

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-blue.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-blue.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "..\COPYING"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_ROOT_FOLDER}\${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
;!define MUI_FINISHPAGE_NOAUTOCLOSE
;!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_NAME}.exe"
;!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!define MUI_FINISHPAGE_LINK_COLOR 0000FF
!define MUI_FINISHPAGE_LINK "Visit the Emulator site for the latest news."
!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Delete Simulator State Files"
!define MUI_FINISHPAGE_RUN_FUNCTION un.DeleteStateFiles
;!define MUI_FINISHPAGE_RUN_NOTCHECKED
!insertmacro MUI_UNPAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

InstType "Full"
InstType "Classic"
InstType "Woodstock"
InstType "Spice"
InstType "Coconut"
InstType "Voyager"

Section "Program Files" SecMain
  SetDetailsPrint textonly
  DetailPrint "Installing Nonpareil Core Files..."
  SetDetailsPrint listonly

  SectionIn 1 2 3 4 5 6 RO
  SetOutPath "$INSTDIR"
  SetOverwrite on
  
  File "..\nonpareil.exe"
  File "..\uasm.exe"
  File "..\modutil.exe"
  File "..\SDL.dll"
  File "..\libxml2.dll"
  File "..\CREDITS"
  File "..\README"
  File "..\COPYING"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Nonpareil.lnk" "$INSTDIR\nonpareil.exe"
  CreateShortCut "$DESKTOP\Nonpareil.lnk" "$INSTDIR\nonpareil.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-35 Skins" SecSkin35
  SetDetailsPrint textonly
  DetailPrint "Installing HP-35 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOverwrite on
  File "..\35.lst"
  File "..\35.obj"
  File "..\35.kml"
  File "..\35.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-35.lnk" "$INSTDIR\nonpareil.exe" "35.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-45 Skins" SecSkin45
  SetDetailsPrint textonly
  DetailPrint "Installing HP-45 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOverwrite on
  File "..\45.lst"
  File "..\45.obj"
  File "..\45.kml"
  File "..\45.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-45.lnk" "$INSTDIR\nonpareil.exe" "45.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-55 Skins" SecSkin55
  SetDetailsPrint textonly
  DetailPrint "Installing HP-55 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOverwrite on
  File "..\55.lst"
  File "..\55.obj"
  File "..\55.kml"
  File "..\55.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-55.lnk" "$INSTDIR\nonpareil.exe" "55.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-80 Skins" SecSkin80
  SetDetailsPrint textonly
  DetailPrint "Installing HP-80 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 2
  SetOverwrite on
  File "..\80.lst"
  File "..\80.obj"
  File "..\80.kml"
  File "..\80.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-80.lnk" "$INSTDIR\nonpareil.exe" "80.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-21 Skins" SecSkin21
  SetDetailsPrint textonly
  DetailPrint "Installing HP-21 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 3
  SetOverwrite on
  File "..\21.obj"
  File "..\21.kml"
  File "..\21.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-21.lnk" "$INSTDIR\nonpareil.exe" "21.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-25 Skins" SecSkin25
  SetDetailsPrint textonly
  DetailPrint "Installing HP-25 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 3
  SetOverwrite on
  File "..\25.obj"
  File "..\25.kml"
  File "..\25.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-25.lnk" "$INSTDIR\nonpareil.exe" "25.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-32E Skins" SecSkin32E
  SetDetailsPrint textonly
  DetailPrint "Installing HP-32E Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\32e.obj"
  File "..\32e.kml"
  File "..\32e.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-32E.lnk" "$INSTDIR\nonpareil.exe" "32e.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-33C Skins" SecSkin33C
  SetDetailsPrint textonly
  DetailPrint "Installing HP-33C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\33c.obj"
  File "..\33c.kml"
  File "..\33c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-33C.lnk" "$INSTDIR\nonpareil.exe" "33c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-34C Skins" SecSkin34C
  SetDetailsPrint textonly
  DetailPrint "Installing HP-34C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\34c.obj"
  File "..\34c.kml"
  File "..\34c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-34C.lnk" "$INSTDIR\nonpareil.exe" "34c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-37E Skins" SecSkin37E
  SetDetailsPrint textonly
  DetailPrint "Installing HP-37E Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\37e.obj"
  File "..\37e.kml"
  File "..\37e.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-37E.lnk" "$INSTDIR\nonpareil.exe" "37e.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-38E Skins" SecSkin38E
  SetDetailsPrint textonly
  DetailPrint "Installing HP-38E Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\38e.obj"
  File "..\38e.kml"
  File "..\38c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-38E.lnk" "$INSTDIR\nonpareil.exe" "38e.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-38C Skins" SecSkin38C
  SetDetailsPrint textonly
  DetailPrint "Installing HP-38C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 4
  SetOverwrite on
  File "..\38c.obj"
  File "..\38c.kml"
  File "..\38c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-38C.lnk" "$INSTDIR\nonpareil.exe" "38C.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-41C/CV/CX Skins" SecSkin41
  SetDetailsPrint textonly
  DetailPrint "Installing HP-41 Skins..."
  SetDetailsPrint listonly

  SectionIn 1 5
  SetOverwrite on
  File "..\41cv.mod"
  File "..\41cx.mod"
  File "..\82182a.mod"
  File "..\82143a.mod"
  File "..\41cv.kml"
  File "..\41cx.kml"
  File "..\41cv.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-41CV.lnk" "$INSTDIR\nonpareil.exe" "41cv.kml"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-41CX.lnk" "$INSTDIR\nonpareil.exe" "41cx.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-11C Skins" SecSkin11c
  SetDetailsPrint textonly
  DetailPrint "Installing HP-11C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 6
  SetOverwrite on
  File "..\11c.obj"
  File "..\11c.kml"
  File "..\11c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-11C.lnk" "$INSTDIR\nonpareil.exe" "11c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-12C Skins" SecSkin12c
  SetDetailsPrint textonly
  DetailPrint "Installing HP-12C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 6
  SetOverwrite on
  File "..\12c.obj"
  File "..\12c.kml"
  File "..\12c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-12C.lnk" "$INSTDIR\nonpareil.exe" "12c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-15C Skins" SecSkin15c
  SetDetailsPrint textonly
  DetailPrint "Installing HP-15C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 6
  SetOverwrite on
  File "..\15c.obj"
  File "..\15c.kml"
  File "..\15c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-15C.lnk" "$INSTDIR\nonpareil.exe" "15c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "HP-16C Skins" SecSkin16c
  SetDetailsPrint textonly
  DetailPrint "Installing HP-16C Skins..."
  SetDetailsPrint listonly

  SectionIn 1 6
  SetOverwrite on
  File "..\16c.obj"
  File "..\16c.kml"
  File "..\16c.png"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\HP-16C.lnk" "$INSTDIR\nonpareil.exe" "16c.kml"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  SetDetailsPrint textonly
  DetailPrint "Creating Shortcuts..."
  SetDetailsPrint listonly

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Readme.lnk" "$INSTDIR\readme"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Emu42.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Emu42.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMain}    "The main program files with documentation."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin35}  "HP-35 Classic Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin45}  "HP-45 Classic Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin55}  "HP-55 Classic Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin80}  "HP-80 Classic Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin21}  "HP-21 Woodstock Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin25}  "HP-25 Woodstock Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin32E} "HP-32E Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin33C} "HP-33C Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin34C} "HP-34C Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin37E} "HP-37E Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin38E} "HP-38E Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin38C} "HP-38C Spice Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin41}  "HP-41C/CV/CX Coconut Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin11c} "HP-11C Voyager Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin12c} "HP-12C Voyager Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin15c} "HP-15C Voyager Series KML script file."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSkin16c} "HP-16C Voyager Series KML script file."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; check if GTK+ 2 library is installed
Function .onInit
  ; try to get installation dir of GTK+ 2 in HKLM
  ReadRegStr $R0 HKLM "Software\GTK\2.0" "Path"
  StrCmp $R0 "" 0 search
    ; try to get installation dir of GTK+ 2 in HKCU
    ReadRegStr $R0 HKCU "Software\GTK\2.0" "Path"
    StrCmp $R0 "" fail
  ; search for dir containing data
  search:
  IfFileExists "$R0\*.*" 0 fail
    return
  ; search failed
  fail:
  !ifdef GTK2                  ; GTK+ 2 installer packet defined
    ClearErrors
    SetOutPath "$TEMP"
    SetOverwrite on
    File "..\${GTK2}"
    ExecWait "$TEMP\${GTK2}"
    Delete "$TEMP\${GTK2}"
    IfErrors 0 +2              ; GTK+ 2 installation successful
      abort                    ; no quitted with cancel
  !else                        ; no GTK+ 2 installer packet defined
    MessageBox MB_OK|MB_ICONSTOP "GTK+ 2 library not found, install first please."
    abort
  !endif
FunctionEnd

; delete application settings
Function un.DeleteStateFiles
  StrCmp $PROFILE "" noprofile
    Delete "$PROFILE\nonpareil\*.*"
    RMDir "$PROFILE\nonpareil"
  noprofile:
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  
  SetDetailsPrint textonly
  DetailPrint "Deleting Files..."
  SetDetailsPrint listonly

  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

; section Program Files
  Delete "$INSTDIR\nonpareil.exe"
  Delete "$INSTDIR\uasm.exe"
  Delete "$INSTDIR\modutil.exe"
  Delete "$INSTDIR\SDL.dll"
  Delete "$INSTDIR\libxml2.dll"
  Delete "$INSTDIR\CREDITS"
  Delete "$INSTDIR\README"
  Delete "$INSTDIR\COPYING"
; section HP-35 skins
  Delete "$INSTDIR\35.lst"
  Delete "$INSTDIR\35.obj"
  Delete "$INSTDIR\35.kml"
  Delete "$INSTDIR\35.png"
; section HP-45 skins
  Delete "$INSTDIR\45.lst"
  Delete "$INSTDIR\45.obj"
  Delete "$INSTDIR\45.kml"
  Delete "$INSTDIR\45.png"
; section HP-55 skins
  Delete "$INSTDIR\55.lst"
  Delete "$INSTDIR\55.obj"
  Delete "$INSTDIR\55.kml"
  Delete "$INSTDIR\55.png"
; section HP-80 skins
  Delete "$INSTDIR\80.lst"
  Delete "$INSTDIR\80.obj"
  Delete "$INSTDIR\80.kml"
  Delete "$INSTDIR\80.png"
; section HP-21 skins
  Delete "$INSTDIR\21.obj"
  Delete "$INSTDIR\21.kml"
  Delete "$INSTDIR\21.png"
; section HP-25 skins
  Delete "$INSTDIR\25.obj"
  Delete "$INSTDIR\25.kml"
  Delete "$INSTDIR\25.png"
; section HP-32E skins
  Delete "$INSTDIR\32e.obj"
  Delete "$INSTDIR\32e.kml"
  Delete "$INSTDIR\32e.png"
; section HP-33C skins
  Delete "$INSTDIR\33c.obj"
  Delete "$INSTDIR\33c.kml"
  Delete "$INSTDIR\33c.png"
; section HP-34C skins
  Delete "$INSTDIR\34c.obj"
  Delete "$INSTDIR\34c.kml"
  Delete "$INSTDIR\34c.png"
; section HP-37E skins
  Delete "$INSTDIR\37e.obj"
  Delete "$INSTDIR\37e.kml"
  Delete "$INSTDIR\37e.png"
; section HP-38E skins
  Delete "$INSTDIR\38e.obj"
  Delete "$INSTDIR\38e.kml"
  Delete "$INSTDIR\38c.png"
; section HP-38C skins
  Delete "$INSTDIR\38c.obj"
  Delete "$INSTDIR\38c.kml"
  Delete "$INSTDIR\38c.png"
; section HP-41 skins
  Delete "$INSTDIR\41cv.mod"
  Delete "$INSTDIR\41cx.mod"
  Delete "$INSTDIR\82182a.mod"
  Delete "$INSTDIR\82143a.mod"
  Delete "$INSTDIR\41cv.kml"
  Delete "$INSTDIR\41cx.kml"
  Delete "$INSTDIR\41cv.png"
; section HP-11C skins
  Delete "$INSTDIR\11c.obj"
  Delete "$INSTDIR\11c.kml"
  Delete "$INSTDIR\11c.png"
; section HP-12C skins
  Delete "$INSTDIR\12c.obj"
  Delete "$INSTDIR\12c.kml"
  Delete "$INSTDIR\12c.png"
; section HP-15C skins
  Delete "$INSTDIR\15c.obj"
  Delete "$INSTDIR\15c.kml"
  Delete "$INSTDIR\15c.png"
; section HP-16C skins
  Delete "$INSTDIR\16c.obj"
  Delete "$INSTDIR\16c.kml"
  Delete "$INSTDIR\16c.png"

  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-35.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-45.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-55.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-80.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-21.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-25.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-32E.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-33C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-34C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-37E.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-38E.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-38C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-41CV.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-41CX.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-11C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-12C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-15C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\HP-16C.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Readme.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Nonpareil.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Readme.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$DESKTOP\Nonpareil.lnk"

; delete ${PRODUCT_ROOT_FOLDER}\${PRODUCT_NAME}
  RMDir "$SMPROGRAMS\$ICONS_GROUP"
; try to delete parent of start menu folder (normally ${PRODUCT_ROOT_FOLDER})
  ${un.GetParent} "$SMPROGRAMS\$ICONS_GROUP" $R0
  RMDir $R0

; delete "$INSTDIR"
  RMDir /r "$INSTDIR"
; try to delete parent of install dir (normally ${PRODUCT_ROOT_FOLDER})
  ${un.GetParent} $INSTDIR $R0
  RMDir $R0

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
