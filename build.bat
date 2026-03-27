@echo off
<<<<<<< HEAD
setlocal

set GPP=D:\tools\winlibs\mingw64\bin\g++.exe
set SFML=D:\tools\SFML-3.0.2
set OUT=MassMadness.exe

echo =============================================
echo   Building Mass Madness...
echo =============================================

%GPP% -fopenmp -std=c++17 -O2 ^
    src\main.cpp ^
    src\Agent.cpp ^
    src\Simulation.cpp ^
    src\Graphics.cpp ^
    -I include ^
    -I "%SFML%\include" ^
    -L "%SFML%\lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system ^
    -o %OUT%

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build SUCCESSFUL!  -->  %OUT%

    echo Copying SFML DLLs...
    copy /Y "%SFML%\bin\sfml-graphics-3.dll" . >nul
    copy /Y "%SFML%\bin\sfml-window-3.dll"   . >nul
    copy /Y "%SFML%\bin\sfml-system-3.dll"   . >nul

    echo.
    echo Launching game...
    start "" %OUT%
) else (
    echo.
    echo BUILD FAILED  --  check errors above
    exit /b 1
)
=======
g++ -fopenmp -std=c++17 -o game.exe src/main.cpp src/Agent.cpp src/Simulation.cpp src/Graphics.cpp -I include -I "D:/SFML-3.0.2/include" -L "D:/SFML-3.0.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -lm
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    game.exe
) else (
    echo Compilation failed!
)
>>>>>>> ac04af5233401e353bf199c9cd553de040cc7d4e
