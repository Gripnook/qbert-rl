#include "game-entity.h"

#include <stdexcept>

namespace Qbert {

bool isDangerousEnemy(const GameEntity& entity)
{
    return entity == GameEntity::Coily || entity == GameEntity::PurpleEnemy ||
        entity == GameEntity::RedEnemy;
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
        return "q";
    case GameEntity::Coily:
        return "c";
    case GameEntity::PurpleEnemy:
        return "p";
    case GameEntity::RedEnemy:
        return "r";
    case GameEntity::GreenEnemy:
        return "g";
    case GameEntity::Disc:
        return "d";
    default:
        throw std::logic_error{"impossible"};
    }
}
}
