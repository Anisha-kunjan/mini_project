@echo off
echo Compiling 2D Graphics Editor...
gcc -O2 -Wall -Wextra graphics_editor.c -o graphics_editor.exe
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)
echo Compilation successful. Running editor...
graphics_editor.exe
