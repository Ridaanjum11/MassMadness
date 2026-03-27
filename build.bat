@echo off
setlocal

REM Change SFML path if installed elsewhere
set GPP=C:\msys64\mingw64\bin\g++.exe
set SFML=C:\msys64\mingw64
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
    echo   2. Extract to: C:\SFML-3.0.2
    echo   3. Read SFML_SETUP.md for detailed instructions
    echo.
    exit /b 1
)

echo Compiling with MSYS2 g++...
C:\msys64\usr\bin\bash.exe -lc "cd /c/Users/Rida\ Anjum/Downloads/madness/MassMadness && /mingw64/bin/g++ -fopenmp -std=c++17 -O2 src/main.cpp src/Agent.cpp src/Simulation.cpp src/Graphics.cpp -I include -I /mingw64/include -L /mingw64/lib -lsfml-graphics -lsfml-window -lsfml-system -o MassMadness.exe"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build SUCCESSFUL!  -->  %OUT%

    echo Copying SFML DLLs...
    copy /Y "%SFML%\bin\libsfml-graphics-3.dll" . >nul 2>&1
    copy /Y "%SFML%\bin\libsfml-window-3.dll"   . >nul 2>&1
    copy /Y "%SFML%\bin\libsfml-system-3.dll"   . >nul 2>&1

    echo.
    echo Launching game...
    start "" %OUT%
) else (
    echo.
    echo BUILD FAILED  --  check errors above
    echo Try updating SFML path in build.bat if installed elsewhere
    exit /b 1
)
