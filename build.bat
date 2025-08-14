@echo off

where cl >nul 2>nul
if errorlevel 1 (
    call vcvarsall.bat x64
)

cl /Zi /Fd:Build\ masDirectorySearch.cpp /Fo:build\ /link /out:build\masDirectorySearch.exe