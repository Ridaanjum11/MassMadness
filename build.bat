@echo off
g++ -fopenmp -std=c++17 -o game.exe src/main.cpp src/Agent.cpp src/Simulation.cpp src/Graphics.cpp -I include -I "D:/SFML-3.0.2/include" -L "D:/SFML-3.0.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -lm
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    game.exe
) else (
    echo Compilation failed!
)