#pragma once

#include <SFML/Graphics.hpp>
#include "Simulation.h"

struct TeamColors {
    sf::Color primary;
    sf::Color light;
    sf::Color dark;
};

class Graphics {
public:
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    // Fixed-size shapes reused each frame
    sf::CircleShape  agentCircle;
    sf::RectangleShape mapBlock;
    sf::RectangleShape hudBar;

    // Team color palette
    static const TeamColors COLORS[NUM_TEAMS + 1]; // +1 for neutral

    Graphics(unsigned int width, unsigned int height);

    bool isOpen() const;
    void processEvents(Simulation* sim);
    void render(Simulation* sim);

private:
    float windowW, windowH;
    float hudHeight;

    // Input state for smooth movement
    bool keyW, keyA, keyS, keyD;

    void drawBackground();
    void drawAgents(Simulation* sim);
    void drawHUD(Simulation* sim);
    void drawGameOver(Simulation* sim);
    void applyPlayerInput(Simulation* sim, float deltaTime);
};
