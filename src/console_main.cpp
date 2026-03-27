#include <iostream>
#include <thread>
#include <chrono>
#include "Simulation.h"

int main() {
    std::cout << "=== Mass Madness - Console Only ===" << std::endl;
    std::cout << "Simulation will run for 10 seconds..." << std::endl;
    std::cout << std::endl;
    
    // Create simulation
    Simulation* sim = new Simulation(200);
    
    // Spawn 100 agents
    for (int i = 0; i < 100; i++) {
        float x = (rand() % 800);
        float y = (rand() % 600);
        sim->spawnAgent(x, y);
    }
    
    std::cout << "Spawned " << sim->getAgentCount() << " agents" << std::endl;
    std::cout << "Leader position: (" << sim->leaderX << ", " << sim->leaderY << ")" << std::endl;
    std::cout << std::endl;
    
    // Run simulation for 10 seconds
    auto start = std::chrono::high_resolution_clock::now();
    float totalTime = 0.0f;
    int frameCount = 0;
    
    while (totalTime < 10.0f) {
        float deltaTime = 0.016f; // 60 FPS
        
        sim->update(deltaTime);
        sim->collectAgents();
        
        frameCount++;
        totalTime += deltaTime;
        
        // Print stats every second
        if (frameCount % 60 == 0) {
            int seconds = (int)totalTime;
            std::cout << "Time: " << seconds << "s | Crowd: " << sim->crowdSize 
                      << "/" << sim->getAgentCount() << " (" 
                      << (sim->crowdSize * 100.0f / sim->getAgentCount()) << "%)" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Total frames: " << frameCount << std::endl;
    std::cout << "Final crowd size: " << sim->crowdSize << " / " << sim->getAgentCount() << std::endl;
    
    delete sim;
    return 0;
}
