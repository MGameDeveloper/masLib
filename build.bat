@echo off

if not exist build ( mkdir build )

where cl >nul 2>nul
if errorlevel 1 (
    call vcvarsall.bat x64
)

cl -Zi -Fd:Build/ masDirectorySearch.cpp -Fo:build/ -link -OUT:build/masDirectorySearch.exe