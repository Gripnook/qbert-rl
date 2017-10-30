#pragma once

#include <array>
#include <utility>

#include "screen.h"
#include "game-entity.h"

namespace Qbert {

template <typename T>
using Grid = std::array<std::array<T, 8>, 8>;

using StateType = std::pair<Grid<GameEntity>, Grid<Color>>;

StateType getState(const Screen& screen);
}
