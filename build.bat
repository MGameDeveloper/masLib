@echo off

if not exist build ( mkdir Build )

where cl >nul 2>nul
if errorlevel 1 (
    call vcvarsall.bat x64
)

cl -Zi -Fd:Build/ test.c mas/src/*.c User32.lib Shlwapi.lib -Fo:Build/ -link -OUT:Build/mas.exe

