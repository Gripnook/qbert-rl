#pragma once

#include <string>

namespace Qbert {

enum class GameEntity
{
    Void,
    None,
    Qbert,
    PurpleEnemy,
    GreenEnemy
};

bool isDangerous(const GameEntity& entity);
std::string toString(const GameEntity& entity);
}
