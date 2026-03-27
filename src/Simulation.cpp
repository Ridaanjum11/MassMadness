#include "../include/Simulation.h"
#include <cmath>
#include <cstdlib>
#include <climits>
#include <cstdint>
#include <algorithm>
#include <omp.h>

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

static float inline sqDist(float ax, float ay, float bx, float by) {
    float dx = bx - ax, dy = by - ay;
    return dx * dx + dy * dy;
}

unsigned int Simulation::fastRand() {
    rngState ^= (rngState << 13);
    rngState ^= (rngState >> 17);
    rngState ^= (rngState << 5);
    return rngState;
}

float Simulation::fastRandFloat(float lo, float hi) {
    return lo + (float)(fastRand() & 0xFFFF) / 65535.f * (hi - lo);
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────

Simulation::Simulation(float w, float h)
    : worldW(w), worldH(h),
      collectRadius(28.f),
      absorptionRadius(18.f),
      followerOrbitRadius(40.f),
      gameTimeRemaining(180.f),
      state(GameState::PLAYING),
      winnerTeam(-1),
      rngState(12345678u)
{
    for (int i = 0; i < NUM_TEAMS; i++) {
        leaders[i]   = nullptr;
        teamSize[i]  = 0;

        aiState[i].targetX      = w * 0.5f;
        aiState[i].targetY      = h * 0.5f;
        aiState[i].decisionTimer = (float)i * 0.7f;
        aiState[i].fleeing      = false;
    }
}

Simulation::~Simulation() {
    for (auto* a : agents) delete a;
    agents.clear();
}

// ─────────────────────────────────────────────────────────────────────────────
// Spawn functions
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::spawnNeutralAgents(int count) {
    agents.reserve(agents.size() + count);
    for (int i = 0; i < count; i++) {
        float x = fastRandFloat(30.f, worldW - 30.f);
        float y = fastRandFloat(30.f, worldH - 30.f);
        Agent* a = new Agent(x, y, TEAM_NEUTRAL, AgentType::NEUTRAL);
        // Give neutrals a random wander direction
        float angle = fastRandFloat(0.f, 6.2832f);
        a->wanderDirX = std::cos(angle);
        a->wanderDirY = std::sin(angle);
        a->speed = fastRandFloat(25.f, 55.f);
        agents.push_back(a);
    }
}

void Simulation::spawnTeam(int team, float leaderX, float leaderY,
                           int numFollowers, bool humanLeader) {
    // Create leader
    Agent* ldr = new Agent(leaderX, leaderY, team, AgentType::TEAM_LEADER);
    ldr->isHumanControlled = humanLeader;
    leaders[team] = ldr;
    teamSize[team]++;
    agents.push_back(ldr);

    // Create followers arranged around leader
    for (int i = 0; i < numFollowers; i++) {
        float angle = (float)i / numFollowers * 6.2832f;
        float ox = std::cos(angle) * 35.f;
        float oy = std::sin(angle) * 35.f;
        Agent* f = new Agent(leaderX + ox, leaderY + oy, team, AgentType::TEAM_FOLLOWER);
        f->formOffsetX = ox;
        f->formOffsetY = oy;
        teamSize[team]++;
        agents.push_back(f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Main update
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::update(float deltaTime) {
    if (state != GameState::PLAYING) return;

    // Tick timer
    gameTimeRemaining -= deltaTime;
    if (gameTimeRemaining <= 0.f) {
        gameTimeRemaining = 0.f;
        checkWinCondition();
        return;
    }

    // AI for non-human leaders
    updateAILeaders(deltaTime);

    // Followers trail their leaders
    updateFollowers(deltaTime);

    // Neutrals wander
    updateNeutralAgents(deltaTime);

    // Move all agents (OpenMP parallelized)
    int n = (int)agents.size();
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        agents[i]->update(deltaTime, worldW, worldH);
    }

    // Collection & absorption (sequential — modifies team data)
    handleCollections();
    handleAbsorptions();

    // Check win every second-ish (when timer crosses integer)
    checkWinCondition();
}

// ─────────────────────────────────────────────────────────────────────────────
// Neutral agent wandering
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::updateNeutralAgents(float deltaTime) {
    int n = (int)agents.size();
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        Agent* a = agents[i];
        if (a->team != TEAM_NEUTRAL) continue;

        // Update wander timer
        a->wanderTimer -= deltaTime;
        if (a->wanderTimer <= 0.f) {
            // Pick a new random wander direction (thread-safe local rng)
            unsigned int lrng = (unsigned int)(size_t)a * 1234567u ^ (unsigned int)(a->x * 100.f);
            lrng ^= (lrng << 13); lrng ^= (lrng >> 17); lrng ^= (lrng << 5);
            float angle = (float)(lrng & 0xFFFF) / 65535.f * 6.2832f;
            a->wanderDirX = std::cos(angle);
            a->wanderDirY = std::sin(angle);
            a->wanderTimer = 1.5f + (float)(lrng & 0xFF) / 255.f * 2.f;
        }

        a->vx = a->wanderDirX * a->speed;
        a->vy = a->wanderDirY * a->speed;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Followers trail their team leader in formation
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::updateFollowers(float deltaTime) {
    int n = (int)agents.size();
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        Agent* a = agents[i];
        if (a->type != AgentType::TEAM_FOLLOWER) continue;
        if (a->team < 0 || a->team >= NUM_TEAMS) continue;

        Agent* ldr = leaders[a->team];
        if (!ldr) continue;

        // Target = leader position + formation offset (slowly rotating)
        float angle = std::atan2(a->formOffsetY, a->formOffsetX);
        angle += deltaTime * 0.4f; // slow orbit
        float r = followerOrbitRadius + (float)((size_t)a & 0xF); // slight variance
        a->formOffsetX = std::cos(angle) * r;
        a->formOffsetY = std::sin(angle) * r;

        float tx = ldr->x + a->formOffsetX;
        float ty = ldr->y + a->formOffsetY;

        float dist = a->distanceTo(tx, ty);
        if (dist > 8.f) {
            a->moveTowards(tx, ty, a->speed + dist * 0.5f);
        } else {
            // Stop jitter
            a->vx *= 0.8f;
            a->vy *= 0.8f;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// AI leader decision making
// ─────────────────────────────────────────────────────────────────────────────

Agent* Simulation::findNearestNeutral(float x, float y) const {
    Agent* best = nullptr;
    float bestSq = 1e18f;
    for (auto* a : agents) {
        if (a->team != TEAM_NEUTRAL) continue;
        float sq = sqDist(x, y, a->x, a->y);
        if (sq < bestSq) { bestSq = sq; best = a; }
    }
    return best;
}

Agent* Simulation::findNearestEnemyLeader(int myTeam, float x, float y) const {
    Agent* best = nullptr;
    float bestSq = 1e18f;
    for (int t = 0; t < NUM_TEAMS; t++) {
        if (t == myTeam || !leaders[t]) continue;
        float sq = sqDist(x, y, leaders[t]->x, leaders[t]->y);
        if (sq < bestSq) { bestSq = sq; best = leaders[t]; }
    }
    return best;
}

int Simulation::findLargestEnemyTeam(int myTeam) const {
    int best = -1, bestSz = 0;
    for (int t = 0; t < NUM_TEAMS; t++) {
        if (t == myTeam) continue;
        if (teamSize[t] > bestSz) { bestSz = teamSize[t]; best = t; }
    }
    return best;
}

void Simulation::updateAILeaders(float deltaTime) {
    for (int t = 0; t < NUM_TEAMS; t++) {
        Agent* ldr = leaders[t];
        if (!ldr || ldr->isHumanControlled) continue;

        TeamAIState& ai = aiState[t];
        ai.decisionTimer -= deltaTime;

        bool needNewTarget = (ai.decisionTimer <= 0.f);

        if (needNewTarget) {
            ai.decisionTimer = fastRandFloat(1.2f, 2.8f);

            // Decision logic:
            // 1) If a neutral is nearby, go for it
            // 2) If we are the biggest team, hunt the nearest enemy leader
            // 3) Otherwise, gather neutrals
            // 4) Flee if massively outnumbered

            int mySize = teamSize[t];
            int biggestEnemy = findLargestEnemyTeam(t);
            int enemySize = (biggestEnemy >= 0) ? teamSize[biggestEnemy] : 0;

            Agent* nearNeutral = findNearestNeutral(ldr->x, ldr->y);

            if (enemySize > mySize * 2 && biggestEnemy >= 0 && leaders[biggestEnemy]) {
                // Flee from largest enemy
                Agent* enemy = leaders[biggestEnemy];
                float dx = ldr->x - enemy->x;
                float dy = ldr->y - enemy->y;
                float d = std::sqrt(dx*dx + dy*dy);
                if (d < 0.1f) d = 1.f;
                ai.targetX = ldr->x + (dx/d) * 250.f;
                ai.targetY = ldr->y + (dy/d) * 250.f;
                ai.fleeing = true;
            } else if (mySize >= enemySize + 3 && biggestEnemy >= 0 && leaders[biggestEnemy]) {
                // Hunt the weakest enemy leader
                int smallest = -1, smallestSz = INT_MAX;
                for (int i = 0; i < NUM_TEAMS; i++) {
                    if (i == t || !leaders[i]) continue;
                    if (teamSize[i] < smallestSz) { smallestSz = teamSize[i]; smallest = i; }
                }
                if (smallest >= 0 && leaders[smallest]) {
                    ai.targetX = leaders[smallest]->x;
                    ai.targetY = leaders[smallest]->y;
                }
                ai.fleeing = false;
            } else if (nearNeutral) {
                // Go for nearest neutral
                ai.targetX = nearNeutral->x;
                ai.targetY = nearNeutral->y;
                ai.fleeing = false;
            } else {
                // Wander to a random point
                ai.targetX = fastRandFloat(50.f, worldW - 50.f);
                ai.targetY = fastRandFloat(50.f, worldH - 50.f);
                ai.fleeing = false;
            }

            // Clamp target to world
            ai.targetX = std::max(20.f, std::min(worldW - 20.f, ai.targetX));
            ai.targetY = std::max(20.f, std::min(worldH - 20.f, ai.targetY));
        }

        // Move AI leader toward its target
        ldr->moveTowards(ai.targetX, ai.targetY, ldr->speed);

        // If very close to target, slow down to avoid jitter
        float d = ldr->distanceTo(ai.targetX, ai.targetY);
        if (d < 15.f) {
            ldr->vx *= 0.3f;
            ldr->vy *= 0.3f;
            aiState[t].decisionTimer = 0.f; // force new target next frame
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Collection: leaders collect neutrals within collect radius
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::handleCollections() {
    float cr2 = collectRadius * collectRadius;

    for (int t = 0; t < NUM_TEAMS; t++) {
        Agent* ldr = leaders[t];
        if (!ldr) continue;

        for (auto* a : agents) {
            if (a->team != TEAM_NEUTRAL) continue;
            float sq = sqDist(ldr->x, ldr->y, a->x, a->y);
            if (sq < cr2) {
                // Convert to this team as a follower
                a->team = t;
                a->type = AgentType::TEAM_FOLLOWER;
                // Assign formation offset
                float angle = fastRandFloat(0.f, 6.2832f);
                float r = followerOrbitRadius + fastRandFloat(-10.f, 15.f);
                a->formOffsetX = std::cos(angle) * r;
                a->formOffsetY = std::sin(angle) * r;
                a->speed = 110.f;
                teamSize[t]++;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Absorption: larger crowd absorbs followers of smaller crowd on contact
// ─────────────────────────────────────────────────────────────────────────────

void Simulation::handleAbsorptions() {
    float ar2 = absorptionRadius * absorptionRadius;

    for (int t = 0; t < NUM_TEAMS; t++) {
        Agent* ldr = leaders[t];
        if (!ldr) continue;

        for (auto* a : agents) {
            // Only absorb enemy followers (not leaders)
            if (a->team == t || a->team == TEAM_NEUTRAL) continue;
            if (a->type == AgentType::TEAM_LEADER) continue;

            float sq = sqDist(ldr->x, ldr->y, a->x, a->y);
            if (sq < ar2 * 4.f) {
                // Only absorb if our team is bigger
                if (teamSize[t] > teamSize[a->team]) {
                    int oldTeam = a->team;
                    teamSize[oldTeam] = std::max(0, teamSize[oldTeam] - 1);
                    a->team = t;
                    a->type = AgentType::TEAM_FOLLOWER;
                    float angle = fastRandFloat(0.f, 6.2832f);
                    float r = followerOrbitRadius + fastRandFloat(-10.f, 15.f);
                    a->formOffsetX = std::cos(angle) * r;
                    a->formOffsetY = std::sin(angle) * r;
                    teamSize[t]++;
                }
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Win condition
// ─────────────────────────────────────────────────────────────────────────────

int Simulation::getNeutralCount() const {
    int cnt = 0;
    for (auto* a : agents)
        if (a->team == TEAM_NEUTRAL) cnt++;
    return cnt;
}

int Simulation::getTotalCitizens() const {
    return (int)agents.size();
}

float Simulation::getTeamPercent(int team) const {
    int total = (int)agents.size();
    if (total == 0) return 0.f;
    return (float)teamSize[team] / (float)total * 100.f;
}

bool Simulation::checkWinCondition() {
    if (state != GameState::PLAYING) return true;

    // Timer expired: most agents wins
    if (gameTimeRemaining <= 0.f) {
        int best = 0;
        for (int i = 1; i < NUM_TEAMS; i++) {
            if (teamSize[i] > teamSize[best]) best = i;
        }
        winnerTeam = best;
        state = GameState::GAME_OVER;
        return true;
    }

    // Early win: one team has >65% of all non-neutral agents
    int nonNeutral = 0;
    for (int t = 0; t < NUM_TEAMS; t++) nonNeutral += teamSize[t];
    if (nonNeutral == 0) return false;

    for (int t = 0; t < NUM_TEAMS; t++) {
        if ((float)teamSize[t] / (float)nonNeutral > 0.65f) {
            winnerTeam = t;
            state = GameState::GAME_OVER;
            return true;
        }
    }
    return false;
}
