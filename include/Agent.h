#pragma once

#include <cmath>
#include <string>

// Team IDs
#define TEAM_RED    0
#define TEAM_GREEN  1
#define TEAM_BLUE   2
#define TEAM_YELLOW 3
#define TEAM_NEUTRAL 4
#define NUM_TEAMS   4

// Agent types
enum class AgentType {
    NEUTRAL,        // White citizen (wandering, can be collected)
    TEAM_LEADER,    // The human player OR AI leader
    TEAM_FOLLOWER   // Bot that follows its team leader
};

class Agent {
public:
    float x, y;           // Position
    float vx, vy;         // Velocity
    float radius;         // Visual radius
    float speed;          // Base movement speed
    int team;             // Which team (0-3 = teams, 4 = neutral)
    AgentType type;       // Neutral / Leader / Follower

    bool isHumanControlled; // true only for the ONE human player

    // Wander state (for neutral agents and AI)
    float wanderTimer;
    float wanderDirX, wanderDirY;

    // Follower formation offset from leader
    float formOffsetX, formOffsetY;

    Agent(float px, float py, int t = TEAM_NEUTRAL, AgentType at = AgentType::NEUTRAL);

    void update(float deltaTime, float worldW, float worldH);
    void setVelocity(float newVx, float newVy);
    void moveTowards(float targetX, float targetY, float spd);
    float distanceTo(float tx, float ty) const;
    float distanceTo(const Agent* other) const;
};
