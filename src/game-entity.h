#pragma once

#include <string>

namespace Qbert {

// An enum representing entities in the game of Q*bert.
enum class GameEntity
{
    Void,
    None,
    Qbert,
    Coily,
    PurpleBall,
    Sam,
    GreenBall,
    RedBall,
    Disc
};

// Is the entity one that can damage Qbert?
bool isDangerousEnemy(const GameEntity& entity);

// Converts the entity to a unique string representation.
std::string toString(const GameEntity& entity);
}
