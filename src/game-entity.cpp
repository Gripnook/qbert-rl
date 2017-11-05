#include "game-entity.h"

#include <stdexcept>

namespace Qbert {

bool isDangerousEnemy(const GameEntity& entity)
{
    return entity == GameEntity::Coily || entity == GameEntity::PurpleBall ||
        entity == GameEntity::RedBall;
}

std::string toString(const GameEntity& entity)
{
    switch (entity)
    {
    case GameEntity::Void:
        return ".";
    case GameEntity::None:
        return "_";
    case GameEntity::Qbert:
        return "Q";
    case GameEntity::Coily:
        return "C";
    case GameEntity::PurpleBall:
        return "p";
    case GameEntity::Sam:
        return "S";
    case GameEntity::GreenBall:
        return "g";
    case GameEntity::RedBall:
        return "r";
    case GameEntity::Disc:
        return "d";
    default:
        throw std::logic_error{"impossible"};
    }
}
}
