#pragma once

#include <vector>
#include <array>
#include "Agent.h"

// Game states
enum class GameState {
    PLAYING,
    GAME_OVER
};

struct TeamAIState {
    float targetX, targetY;     // Current AI navigation target
    float decisionTimer;        // Time until next AI decision
    bool fleeing;               // Is this team currently fleeing?
};

class Simulation {
public:
    // World dimensions
    float worldW, worldH;

    // All agents in the simulation
    std::vector<Agent*> agents;

    // Leader pointers (one per team)
    Agent* leaders[NUM_TEAMS];

    // Team sizes (number of agents on each team including leader)
    int teamSize[NUM_TEAMS];

    // Game settings
    float collectRadius;        // Range within which a leader collects neutrals
    float absorptionRadius;     // Range for team-vs-team absorption
    float followerOrbitRadius;  // Followers orbit leader within this radius

    // Timer
    float gameTimeRemaining;    // Seconds
    GameState state;
    int winnerTeam;             // Set when game ends

    // AI state per team
    TeamAIState aiState[NUM_TEAMS];

    // Random seed state
    unsigned int rngState;

    Simulation(float w, float h);
    ~Simulation();

    // Setup
    void spawnNeutralAgents(int count);
    void spawnTeam(int team, float leaderX, float leaderY, int numFollowers, bool humanLeader);

    // Per-frame update (uses OpenMP internally)
    void update(float deltaTime);

    // Queries
    int getNeutralCount() const;
    int getTotalCitizens() const;   // non-team agents
    float getTeamPercent(int team) const;
    bool checkWinCondition();

private:
    void updateNeutralAgents(float deltaTime);
    void updateFollowers(float deltaTime);
    void updateAILeaders(float deltaTime);
    void handleCollections();
    void handleAbsorptions();
    void enforceBounds(Agent* a);

    // AI helpers
    Agent* findNearestNeutral(float x, float y) const;
    Agent* findNearestEnemyLeader(int myTeam, float x, float y) const;
    int    findLargestEnemyTeam(int myTeam) const;
    float  fastRandFloat(float lo, float hi);
    unsigned int fastRand();
};