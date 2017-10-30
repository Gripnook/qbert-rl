#include "game-entity.h"

#include <stdexcept>

namespace Qbert {

bool isDangerous(const GameEntity& entity)
{
    return entity == GameEntity::Void || entity == GameEntity::PurpleEnemy;
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
    case GameEntity::PurpleEnemy:
        return "p";
    case GameEntity::GreenEnemy:
        return "g";
    default:
        throw std::logic_error{"impossible"};
    }
}
}
