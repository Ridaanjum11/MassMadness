@echo off
setlocal

REM Compiler selection (required)
set GPP=C:\mingw64\bin\g++.exe
if not exist "%GPP%" set GPP=C:\msys64\mingw64\bin\g++.exe

REM Explicit SFML 3.0.2 path (user downloaded version)
set SFML=C:\SFML-3.0.2-Windows.MinGW.x64\SFML-3.0.2
if not exist "%SFML%\include\SFML\Graphics.hpp" (
    echo [ERROR] SFML 3.0.2 not found in %SFML%
    echo Please install and extract SFML 3.0.2 to C:\SFML-3.0.2-Windows.MinGW.x64\SFML-3.0.2
    exit /b 1
)

set OUT=MassMadness.exe

echo =============================================
echo   Building MassMadness...
echo =============================================

REM Check if SFML is installed
if not exist "%SFML%\include\SFML\Graphics.hpp" (
    echo.
    echo [ERROR] SFML not found at: %SFML%
    echo.
    echo Please install SFML 3.0.2:
    echo   1. Download from: https://github.com/SFML/SFML/releases/tag/3.0.2
    echo   2. Extract to: C:\SFML-3.0.2-Windows.MinGW.x64
    echo   3. Read SFML_SETUP.md for detailed instructions
    echo.
    exit /b 1
)

echo Compiling with g++...
if exist "%OUT%" del /Q "%OUT%"
"%GPP%" -fopenmp -std=c++17 -O2 src\main.cpp src\Agent.cpp src\Simulation.cpp src\Graphics.cpp -I include -I "%SFML%\include" -L "%SFML%\lib" -lsfml-graphics -lsfml-window -lsfml-system -o "%OUT%"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build SUCCESSFUL!  - done - %OUT%

    echo Copying SFML DLLs...
    copy /Y "%SFML%\bin\sfml-graphics-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\sfml-window-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\sfml-system-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\libsfml-graphics-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\libsfml-window-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\libsfml-system-3.dll" . >nul 2>&1

    echo.
    echo Launching game...
    start "" "%CD%\%OUT%"
) else (
    echo.
    echo BUILD FAILED  --  check errors above
    echo Try updating SFML path in build.bat if installed elsewhere
    exit /b 1
)
