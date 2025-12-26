@echo off
REM --- Automatic compile and run script for assignment.cpp on Windows ---

REM Check if assignment.cpp exists
IF NOT EXIST "assignment.cpp" (
    echo assignment.cpp not found in this folder!
    pause
    exit /b
)

REM Compile assignment.cpp
g++ assignment.cpp -std=c++17 -o assignment.exe

REM Check if compilation succeeded
IF %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b
)

echo Compilation succeeded! Running program...
echo ----------------------------------------
assignment.exe
echo ----------------------------------------
pause
