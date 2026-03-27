#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "../include/Simulation.h"
#include "../include/GraphicsGDI.h"

// Color names for teams
const char* teamNames[] = { "Red", "Green", "Blue", "Yellow" };

int main() {
    try {
        std::cout << "=== Crowd City - 4 Team Battle ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Red (P1):    W/A/S/D" << std::endl;
        std::cout << "  Green (P2):  Arrow Keys" << std::endl;
        std::cout << "  Blue (P3):   U/H/J/K" << std::endl;
        std::cout << "  Yellow (P4): I/L/O/P" << std::endl;
        std::cout << "  Objective: Recruit white citizens and eliminate other teams!" << std::endl;
        std::cout.flush();
        
        GraphicsGDI* graphics = new GraphicsGDI();
        if (!graphics->isOpen()) {
            std::cout << "ERROR: Graphics initialization failed!" << std::endl;
            delete graphics;
            return 1;
        }
        
        std::cout << "Creating simulation..." << std::endl;
        Simulation* sim = new Simulation(400);
        
        // Spawn 4 player agents at corners
        sim->spawnPlayer(TEAM_RED, 100, 100);        // Top-left
        sim->spawnPlayer(TEAM_GREEN, 700, 100);      // Top-right
        sim->spawnPlayer(TEAM_BLUE, 100, 500);       // Bottom-left
        sim->spawnPlayer(TEAM_YELLOW, 700, 500);     // Bottom-right
        
        std::cout << "Spawning 200 neutral citizens..." << std::endl;
        
        srand((unsigned)time(NULL));
        for (int i = 0; i < 200; i++) {
            float x = 50 + (rand() % 700);
            float y = 50 + (rand() % 500);
            sim->spawnAgent(x, y, TEAM_NEUTRAL);
        }
        
        std::cout << "Starting game..." << std::endl;
        std::cout.flush();
        
        int frame = 0;
        bool gameOver = false;
        
        while (graphics->isOpen() && !gameOver) {
            float deltaTime = 0.016f; // 60 FPS
            
            // Handle player inputs
            graphics->handleEvents(sim);
            
            if (GetAsyncKeyState('W') & 0x8000) sim->players[TEAM_RED]->y -= 3.0f;
            if (GetAsyncKeyState('S') & 0x8000) sim->players[TEAM_RED]->y += 3.0f;
            if (GetAsyncKeyState('A') & 0x8000) sim->players[TEAM_RED]->x -= 3.0f;
            if (GetAsyncKeyState('D') & 0x8000) sim->players[TEAM_RED]->x += 3.0f;
            
            if (GetAsyncKeyState(VK_UP) & 0x8000) sim->players[TEAM_GREEN]->y -= 3.0f;
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) sim->players[TEAM_GREEN]->y += 3.0f;
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) sim->players[TEAM_GREEN]->x -= 3.0f;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) sim->players[TEAM_GREEN]->x += 3.0f;
            
            if (GetAsyncKeyState('U') & 0x8000) sim->players[TEAM_BLUE]->y -= 3.0f;
            if (GetAsyncKeyState('J') & 0x8000) sim->players[TEAM_BLUE]->y += 3.0f;
            if (GetAsyncKeyState('H') & 0x8000) sim->players[TEAM_BLUE]->x -= 3.0f;
            if (GetAsyncKeyState('K') & 0x8000) sim->players[TEAM_BLUE]->x += 3.0f;
            
            if (GetAsyncKeyState('I') & 0x8000) sim->players[TEAM_YELLOW]->y -= 3.0f;
            if (GetAsyncKeyState('O') & 0x8000) sim->players[TEAM_YELLOW]->y += 3.0f;
            if (GetAsyncKeyState('L') & 0x8000) sim->players[TEAM_YELLOW]->x -= 3.0f;
            if (GetAsyncKeyState('P') & 0x8000) sim->players[TEAM_YELLOW]->x += 3.0f;
            
            // Keep players in bounds
            for (int i = 0; i < 4; i++) {
                if (sim->players[i]->x < 30) sim->players[i]->x = 30;
                if (sim->players[i]->x > 770) sim->players[i]->x = 770;
                if (sim->players[i]->y < 30) sim->players[i]->y = 30;
                if (sim->players[i]->y > 570) sim->players[i]->y = 570;
            }
            
            // Update simulation
            sim->update(deltaTime);
            sim->handleCollisions();
            
            // Render
            graphics->renderCrowdCity(sim);
            
            // Print stats every 30 frames
            frame++;
            if (frame % 30 == 0) {
                std::cout << "Red: " << sim->getTeamSize(TEAM_RED) 
                          << " | Green: " << sim->getTeamSize(TEAM_GREEN)
                          << " | Blue: " << sim->getTeamSize(TEAM_BLUE)
                          << " | Yellow: " << sim->getTeamSize(TEAM_YELLOW) << std::endl;
                std::cout.flush();
                
                // Check win condition
                int neutral = sim->getTeamSize(TEAM_NEUTRAL);
                if (neutral == 0) {
                    int winner = sim->getWinningTeam();
                    std::cout << "\n*** " << teamNames[winner] << " TEAM WINS! ***" << std::endl;
                    gameOver = true;
                }
            }
        }
        
        std::cout << "Game ended!" << std::endl;
        
        delete graphics;
        delete sim;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
