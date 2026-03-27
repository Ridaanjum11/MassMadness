#include "../include/Graphics.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

// ─────────────────────────────────────────────────────────────────────────────
// Color palette
// ─────────────────────────────────────────────────────────────────────────────

const TeamColors Graphics::COLORS[NUM_TEAMS + 1] = {
    // Red team
    { sf::Color(220,  50,  50), sf::Color(255, 100, 100), sf::Color(150,  20,  20) },
    // Green team
    { sf::Color( 50, 200,  80), sf::Color(100, 255, 130), sf::Color( 20, 140,  40) },
    // Blue team
    { sf::Color( 60, 100, 220), sf::Color(100, 150, 255), sf::Color( 20,  50, 160) },
    // Yellow team
    { sf::Color(230, 200,  30), sf::Color(255, 240,  80), sf::Color(160, 130,   0) },
    // Neutral
    { sf::Color(220, 220, 220), sf::Color(255, 255, 255), sf::Color(150, 150, 150) },
};

static const char* TEAM_NAMES[NUM_TEAMS] = { "RED", "GREEN", "BLUE", "YELLOW" };

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

Graphics::Graphics(unsigned int width, unsigned int height)
    : fontLoaded(false), windowW((float)width), windowH((float)height),
      hudHeight(64.f),
      keyW(false), keyA(false), keyS(false), keyD(false)
{
    window.create(sf::VideoMode({width, height}), "Mass Madness - Crowd Simulation");
    window.setFramerateLimit(60);

    // Try loading a system font (Arial or fallback)
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/calibri.ttf")) {
            fontLoaded = false;
        } else fontLoaded = true;
    } else fontLoaded = true;

    agentCircle.setPointCount(12);
    hudBar.setSize({1.f, 1.f});
    mapBlock.setSize({1.f, 1.f});
}

bool Graphics::isOpen() const {
    return window.isOpen();
}

// ─────────────────────────────────────────────────────────────────────────────
// Event handling + player input
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::processEvents(Simulation* sim) {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (auto* ke = event->getIf<sf::Event::KeyPressed>()) {
            switch (ke->code) {
                case sf::Keyboard::Key::W: keyW = true;  break;
                case sf::Keyboard::Key::S: keyS = true;  break;
                case sf::Keyboard::Key::A: keyA = true;  break;
                case sf::Keyboard::Key::D: keyD = true;  break;
                case sf::Keyboard::Key::Escape: window.close(); break;
                default: break;
            }
        }
        if (auto* ke = event->getIf<sf::Event::KeyReleased>()) {
            switch (ke->code) {
                case sf::Keyboard::Key::W: keyW = false; break;
                case sf::Keyboard::Key::S: keyS = false; break;
                case sf::Keyboard::Key::A: keyA = false; break;
                case sf::Keyboard::Key::D: keyD = false; break;
                default: break;
            }
        }
    }
    applyPlayerInput(sim, 1.f / 60.f);
}

void Graphics::applyPlayerInput(Simulation* sim, float /*deltaTime*/) {
    Agent* player = sim->leaders[TEAM_RED];
    if (!player || !player->isHumanControlled) return;

    float spd = player->speed;
    float vx = 0.f, vy = 0.f;
    if (keyW) vy -= spd;
    if (keyS) vy += spd;
    if (keyA) vx -= spd;
    if (keyD) vx += spd;

    // Normalize diagonal
    if (vx != 0.f && vy != 0.f) {
        vx *= 0.7071f;
        vy *= 0.7071f;
    }
    player->setVelocity(vx, vy);
}

// ─────────────────────────────────────────────────────────────────────────────
// Background (city grid)
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::drawBackground() {
    float mapY = hudHeight;

    // Dark city ground
    sf::RectangleShape bg({windowW, windowH - mapY});
    bg.setPosition({0.f, mapY});
    bg.setFillColor(sf::Color(18, 22, 30));
    window.draw(bg);

    // Road grid lines
    float cellW = 120.f, cellH = 90.f;
    sf::RectangleShape hLine({windowW, 2.f});
    hLine.setFillColor(sf::Color(38, 46, 60));

    sf::RectangleShape vLine({2.f, windowH - mapY});
    vLine.setFillColor(sf::Color(38, 46, 60));

    // Horizontal roads
    for (float y = mapY; y < windowH; y += cellH) {
        hLine.setPosition({0.f, y});
        window.draw(hLine);
    }
    // Vertical roads
    for (float x = 0; x < windowW; x += cellW) {
        vLine.setPosition({x, mapY});
        window.draw(vLine);
    }

    // City blocks (slightly lighter squares inside the grid)
    sf::RectangleShape block({cellW - 16.f, cellH - 14.f});
    block.setFillColor(sf::Color(24, 30, 42));
    for (float y = mapY; y < windowH - cellH; y += cellH) {
        for (float x = 0; x < windowW - cellW; x += cellW) {
            block.setPosition({x + 9.f, y + 7.f});
            window.draw(block);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Agent rendering
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::drawAgents(Simulation* sim) {
    for (auto* a : sim->agents) {
        float r = a->radius;
        agentCircle.setRadius(r);
        agentCircle.setOrigin({r, r});
        agentCircle.setPosition({a->x, a->y});

        int colorIdx = (a->team >= 0 && a->team < NUM_TEAMS) ? a->team : NUM_TEAMS;

        if (a->team == TEAM_NEUTRAL) {
            agentCircle.setFillColor(COLORS[NUM_TEAMS].primary);
            agentCircle.setOutlineColor(COLORS[NUM_TEAMS].dark);
            agentCircle.setOutlineThickness(1.f);
        } else {
            // Team colored
            if (a->type == AgentType::TEAM_LEADER) {
                agentCircle.setFillColor(COLORS[colorIdx].light);
                agentCircle.setOutlineColor(sf::Color::White);
                agentCircle.setOutlineThickness(2.5f);
            } else {
                agentCircle.setFillColor(COLORS[colorIdx].primary);
                agentCircle.setOutlineColor(COLORS[colorIdx].dark);
                agentCircle.setOutlineThickness(1.2f);
            }
        }
        window.draw(agentCircle);

        // Draw a crown/star indicator on the human player
        if (a->isHumanControlled && fontLoaded) {
            sf::Text crown(font, "*", 14);
            crown.setFillColor(sf::Color::White);
            crown.setPosition({a->x - 5.f, a->y - a->radius - 16.f});
            window.draw(crown);
        }

        // Draw collect radius ring around human leader
        if (a->isHumanControlled) {
            sf::CircleShape ring(sim->collectRadius);
            ring.setOrigin({sim->collectRadius, sim->collectRadius});
            ring.setPosition({a->x, a->y});
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255, 80, 80, 60));
            ring.setOutlineThickness(1.5f);
            window.draw(ring);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// HUD
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::drawHUD(Simulation* sim) {
    // Background bar
    sf::RectangleShape hudBg({windowW, hudHeight});
    hudBg.setPosition({0.f, 0.f});
    hudBg.setFillColor(sf::Color(12, 14, 20));
    window.draw(hudBg);

    // Separator line
    sf::RectangleShape sep({windowW, 2.f});
    sep.setPosition({0.f, hudHeight - 2.f});
    sep.setFillColor(sf::Color(60, 70, 90));
    window.draw(sep);

    // Timer
    if (fontLoaded) {
        int secs = (int)sim->gameTimeRemaining;
        int mins = secs / 60;
        int s    = secs % 60;

        std::ostringstream oss;
        oss << "TIME  " << std::setw(2) << std::setfill('0') << mins
            << ":" << std::setw(2) << std::setfill('0') << s;

        sf::Text timerText(font, oss.str(), 22);
        timerText.setFillColor(sf::Color(200, 210, 230));
        timerText.setPosition({windowW / 2.f - 70.f, 8.f});
        window.draw(timerText);

        sf::Text ctrlText(font, "WASD to move  |  MASS MADNESS", 13);
        ctrlText.setFillColor(sf::Color(80, 100, 130));
        ctrlText.setPosition({windowW / 2.f - 115.f, 36.f});
        window.draw(ctrlText);
    }

    // Team score bars (one per team on left/right sides)
    float barW  = (windowW / 4.f) - 10.f;
    float barH  = 18.f;
    float barY  = 6.f;

    int totalAgents = (int)sim->agents.size();

    // Layout: [RED][GREEN]..timer..[BLUE][YELLOW]
    int teamOrder[NUM_TEAMS] = { TEAM_RED, TEAM_GREEN, TEAM_BLUE, TEAM_YELLOW };
    float barPositions[NUM_TEAMS] = {
        4.f,
        barW + 8.f,
        windowW - 2.f * barW - 8.f,
        windowW - barW - 4.f
    };

    for (int i = 0; i < NUM_TEAMS; i++) {
        int t = teamOrder[i];
        float bx = barPositions[i];

        // Background
        sf::RectangleShape bg({barW, barH + 8.f});
        bg.setPosition({bx, barY});
        bg.setFillColor(sf::Color(30, 36, 48));
        window.draw(bg);

        // Fill based on count
        float ratio = (totalAgents > 0) ? (float)sim->teamSize[t] / (float)totalAgents : 0.f;
        sf::RectangleShape fill({barW * ratio, barH + 8.f});
        fill.setPosition({bx, barY});
        fill.setFillColor(COLORS[t].dark);
        window.draw(fill);

        // Outline
        sf::RectangleShape outline({barW, barH + 8.f});
        outline.setPosition({bx, barY});
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor(COLORS[t].primary);
        outline.setOutlineThickness(1.5f);
        window.draw(outline);

        // Label
        if (fontLoaded) {
            std::string label = std::string(TEAM_NAMES[t]) + " " + std::to_string(sim->teamSize[t]);
            if (t == TEAM_RED) label += " [YOU]";
            sf::Text lbl(font, label, 12);
            lbl.setFillColor(COLORS[t].light);
            lbl.setPosition({bx + 3.f, barY + 1.f});
            window.draw(lbl);

            // Count below bar
            sf::Text cnt(font, std::to_string(sim->teamSize[t]) + " agents", 10);
            cnt.setFillColor(sf::Color(140, 150, 170));
            cnt.setPosition({bx + 3.f, barY + barH + 8.f + 2.f});
            window.draw(cnt);
        }
    }

    // Neutral count (small, centered below timer)
    if (fontLoaded) {
        int neutral = sim->getNeutralCount();
        sf::Text nText(font, "Neutral: " + std::to_string(neutral), 12);
        nText.setFillColor(sf::Color(150, 160, 180));
        nText.setPosition({windowW / 2.f - 35.f, 52.f});
        window.draw(nText);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Game Over screen
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::drawGameOver(Simulation* sim) {
    // Semi-transparent overlay
    sf::RectangleShape overlay({windowW, windowH});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    if (!fontLoaded) return;

    int winner = sim->winnerTeam;
    std::string wName = (winner >= 0 && winner < NUM_TEAMS) ? TEAM_NAMES[winner] : "??";
    sf::Color wCol = (winner >= 0 && winner < NUM_TEAMS) ? COLORS[winner].light : sf::Color::White;

    sf::Text gameOverText(font, "GAME OVER", 60);
    gameOverText.setFillColor(sf::Color::White);
    auto gb = gameOverText.getLocalBounds();
    gameOverText.setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
    gameOverText.setPosition({windowW / 2.f, windowH / 2.f - 80.f});
    window.draw(gameOverText);

    std::string winStr = wName + " TEAM WINS!";
    if (winner == TEAM_RED) winStr = "YOU WIN!  (Red Team)";
    sf::Text winText(font, winStr, 40);
    winText.setFillColor(wCol);
    auto wb = winText.getLocalBounds();
    winText.setOrigin({wb.size.x / 2.f, wb.size.y / 2.f});
    winText.setPosition({windowW / 2.f, windowH / 2.f});
    window.draw(winText);

    // Final scores
    float sy = windowH / 2.f + 60.f;
    for (int t = 0; t < NUM_TEAMS; t++) {
        std::string sc = std::string(TEAM_NAMES[t]) + ": " + std::to_string(sim->teamSize[t]) + " agents";
        sf::Text scoreText(font, sc, 22);
        scoreText.setFillColor(COLORS[t].primary);
        auto sb = scoreText.getLocalBounds();
        scoreText.setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
        scoreText.setPosition({windowW / 2.f, sy});
        window.draw(scoreText);
        sy += 32.f;
    }

    sf::Text closeText(font, "Press ESC to exit", 18);
    closeText.setFillColor(sf::Color(120, 130, 150));
    auto cb = closeText.getLocalBounds();
    closeText.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
    closeText.setPosition({windowW / 2.f, windowH - 40.f});
    window.draw(closeText);
}

// ─────────────────────────────────────────────────────────────────────────────
// Main render
// ─────────────────────────────────────────────────────────────────────────────

void Graphics::render(Simulation* sim) {
    window.clear(sf::Color(10, 12, 18));

    drawBackground();
    drawAgents(sim);
    drawHUD(sim);

    if (sim->state == GameState::GAME_OVER) {
        drawGameOver(sim);
    }

    window.display();
}