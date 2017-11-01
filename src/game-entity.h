#pragma once

#include <string>

namespace Qbert {

enum class GameEntity
{
    Void,
    None,
    Qbert,
    PurpleEnemy,
    RedEnemy,
    GreenEnemy,
    Disc
};

bool isDangerousEnemy(const GameEntity& entity);
std::string toString(const GameEntity& entity);
}
