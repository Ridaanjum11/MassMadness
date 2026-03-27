#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../include/Simulation.h"
#include "../include/Graphics.h"

int main() {
    std::cout << "====================================================\n";
    std::cout << "   MASS MADNESS - Crowd Simulation Game\n";
    std::cout << "====================================================\n";
    std::cout << "Controls:\n";
    std::cout << "  W / A / S / D  -- Move your Red leader\n";
    std::cout << "  ESC            -- Quit\n";
    std::cout << "\n";
    std::cout << "Goal: Collect white citizens to grow your Red crowd!\n";
    std::cout << "      Absorb enemy followers when you outnumber them.\n";
    std::cout << "      3-minute timer -- biggest crowd wins!\n";
    std::cout << "====================================================\n";
    std::cout.flush();

    const float WORLD_W = 1200.f;
    const float WORLD_H = 800.f;

    try {
        // ── Graphics ──────────────────────────────────────────────────────
        Graphics gfx((unsigned)WORLD_W, (unsigned)WORLD_H);
        if (!gfx.isOpen()) {
            std::cerr << "ERROR: Could not open window!\n";
            return 1;
        }
        std::cout << "Window opened.\n";

        // ── Simulation ────────────────────────────────────────────────────
        Simulation sim(WORLD_W, WORLD_H);

        // Team spawn positions (4 corners)
        // RED  — human player, top-left
        sim.spawnTeam(TEAM_RED,     150.f,  130.f, 5, true  /*human*/);

        // GREEN — AI, top-right
        sim.spawnTeam(TEAM_GREEN,  1050.f,  130.f, 5, false);

        // BLUE  — AI, bottom-left
        sim.spawnTeam(TEAM_BLUE,    150.f,  670.f, 5, false);

        // YELLOW — AI, bottom-right
        sim.spawnTeam(TEAM_YELLOW, 1050.f,  670.f, 5, false);

        // Neutral citizens scattered across the map
        sim.spawnNeutralAgents(220);

        std::cout << "Agents: " << sim.agents.size() << " total\n";
        std::cout << "Starting game loop...\n\n";
        std::cout.flush();

        // ── Game Loop ─────────────────────────────────────────────────────
        sf::Clock clock;
        int frameCount = 0;

        while (gfx.isOpen()) {
            float dt = clock.restart().asSeconds();
            // Clamp to avoid huge spikes on first frame or lag
            if (dt > 0.05f) dt = 0.05f;

            // Process SFML events + human input
            gfx.processEvents(&sim);

            // Update simulation (parallel)
            sim.update(dt);

            // Render
            gfx.render(&sim);

            // Console stats every 120 frames (~2s)
            frameCount++;
            if (frameCount % 120 == 0) {
                int neutral = sim.getNeutralCount();
                int secs    = (int)sim.gameTimeRemaining;
                std::cout << "Time: " << secs/60 << "m" << secs%60 << "s"
                          << "  | RED:"    << sim.teamSize[TEAM_RED]
                          << "  GREEN:"    << sim.teamSize[TEAM_GREEN]
                          << "  BLUE:"     << sim.teamSize[TEAM_BLUE]
                          << "  YELLOW:"   << sim.teamSize[TEAM_YELLOW]
                          << "  Neutral:"  << neutral
                          << "\n";
                std::cout.flush();
            }

            // After game over, keep showing screen until ESC/close
            if (sim.state == GameState::GAME_OVER && frameCount % 60 == 0) {
                static bool printed = false;
                if (!printed) {
                    const char* teamNames[] = { "RED", "GREEN", "BLUE", "YELLOW" };
                    std::cout << "\n*** GAME OVER ***\n";
                    if (sim.winnerTeam == TEAM_RED)
                        std::cout << "*** YOU WIN! (Red Team) ***\n";
                    else
                        std::cout << "*** " << teamNames[sim.winnerTeam] << " TEAM WINS! ***\n";
                    printed = true;
                }
            }
        }

        std::cout << "\nGame closed. Final scores:\n";
        for (int t = 0; t < NUM_TEAMS; t++) {
            const char* names[] = { "Red", "Green", "Blue", "Yellow" };
            std::cout << "  " << names[t] << ": " << sim.teamSize[t] << " agents\n";
        }
        std::cout.flush();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown error!\n";
        return 1;
    }

    return 0;
}
