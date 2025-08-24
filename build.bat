@echo off

if not exist build     ( mkdir "build"     )
if not exist build/obj ( mkdir "build/obj" )

where cl >nul 2>nul
if errorlevel 1 (
    call vcvarsall.bat x64
)

:: FRAMEWORK SROUCE AND HEADER FIELS 
set MAS_SRC=                   ^
mas/src/masLog.c               ^
mas/src/masMemory.c            ^
mas/src/masEvent.c             ^
mas/src/masWindow.c            ^
mas/src/masInputController.c   ^
mas/src/masInput.c             ^
mas/src/masTime.c              ^
mas/src/masDirectory.c         ^
mas/src/mas.c

set MAS_HDR= ^
/I"mas/src"  ^ 
/I"mas"           


:: FRAMWORK'S USER'S SOURCE AND HEADER FILES
set USER_MACROS=/DMAS_UNICODE 

set USER_SRC=test.c  
set USER_HDR=
set USER_LIBS=


:: COMPILER OPTTIONS AND CONFIGURATION
set MACROS_DEFINE= ^
%USER_MACROS% 

set COMPILE_OPTION= ^
/Zi                 ^
/Fd:"build/"        ^
/Fo:"build/obj/" 

set LINK_OPTION= /OUT:"build/mas.exe" /SUBSYSTEM:CONSOLE 
set LIBS=User32.lib Shlwapi.lib %USER_LIBS% 
set HDRS=%MAS_HDR% %USER_HDR%  
set SRCS=%USER_SRC% %MAS_SRC%  



:: INVOKING COMPILER
cl %COMPILE_OPTION% %MACROS_DEFINE% %HDRS% %SRCS% %LIBS% /link %LINK_OPTION%