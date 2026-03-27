#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../include/Simulation.h"
#include "../include/GraphicsGDI.h"

int main(int argc, char** argv) {
    try {
        std::cout << "=== Mass Madness - Crowd Simulation (GDI) ===" << std::endl;
        std::cout << "Controls: Arrow Keys - Move Leader, Close Window - Quit" << std::endl;
        std::cout.flush();
        
        GraphicsGDI* graphics = new GraphicsGDI();
        if (!graphics->isOpen()) {
            std::cout << "ERROR: Graphics initialization failed!" << std::endl;
            delete graphics;
            return 1;
        }
        
        std::cout << "Creating simulation..." << std::endl;
        Simulation* sim = new Simulation(200);
        
        // Spawn 100 neutral agents
        srand((unsigned)time(NULL));
        for (int i = 0; i < 100; i++) {
            float x = 50 + (rand() % 700);
            float y = 50 + (rand() % 500);
            sim->spawnAgent(x, y, TEAM_NEUTRAL);
        }
        
        // Spawn 1 player as red team
        sim->spawnPlayer(TEAM_RED, 400, 300);
        
        std::cout << "Spawned " << sim->getAgentCount() << " agents" << std::endl;
        std::cout << "Starting game loop..." << std::endl;
        std::cout.flush();
        
        int frame = 0;
        
        while (graphics->isOpen()) {
            float deltaTime = 0.016f;
            
            graphics->handleEvents(sim);
            sim->update(deltaTime);
            sim->handleCollisions();
            graphics->renderCrowdCity(sim);
            
            frame++;
            if (frame % 60 == 0) {
                std::cout << "Frame " << frame << " | Red Team: " << sim->getTeamSize(TEAM_RED) 
                          << " | Neutral: " << sim->getTeamSize(TEAM_NEUTRAL) << std::endl;
                std::cout.flush();
            }
        }
        
        std::cout << "Game closed. Final red team size: " << sim->getTeamSize(TEAM_RED) << std::endl;
        
        delete graphics;
        delete sim;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
