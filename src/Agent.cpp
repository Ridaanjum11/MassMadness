#include "../include/Agent.h"
#include <cmath>

Agent::Agent(float px, float py, int t, AgentType at)
    : x(px), y(py), vx(0.f), vy(0.f),
      radius(5.f), speed(90.f),
      team(t), type(at),
      isHumanControlled(false),
      wanderTimer(0.f), wanderDirX(1.f), wanderDirY(0.f),
      formOffsetX(0.f), formOffsetY(0.f)
{
    // Leaders are slightly bigger and faster
    if (type == AgentType::TEAM_LEADER) {
        radius = 9.f;
        speed  = 130.f;
    }
    // Followers are slightly slower
    if (type == AgentType::TEAM_FOLLOWER) {
        speed = 110.f;
    }
}

float Agent::distanceTo(float tx, float ty) const {
    float dx = tx - x;
    float dy = ty - y;
    return std::sqrt(dx * dx + dy * dy);
}

float Agent::distanceTo(const Agent* other) const {
    return distanceTo(other->x, other->y);
}

void Agent::setVelocity(float newVx, float newVy) {
    vx = newVx;
    vy = newVy;
}

void Agent::moveTowards(float targetX, float targetY, float spd) {
    float dx = targetX - x;
    float dy = targetY - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > 1.f) {
        vx = (dx / dist) * spd;
        vy = (dy / dist) * spd;
    } else {
        vx = 0.f;
        vy = 0.f;
    }
}

void Agent::update(float deltaTime, float worldW, float worldH) {
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Bounce off world edges
    float margin = radius + 2.f;
    if (x < margin)       { x = margin;        vx = std::abs(vx); }
    if (x > worldW - margin) { x = worldW - margin; vx = -std::abs(vx); }
    if (y < margin)       { y = margin;        vy = std::abs(vy); }
    if (y > worldH - margin) { y = worldH - margin; vy = -std::abs(vy); }
}
