#pragma once

#include <array>
#include <utility>

#include <ale/ale_interface.hpp>

#include "game-entity.h"

namespace Qbert {

using Color = int;

template <typename T>
using Grid = std::array<std::array<T, 8>, 8>;

using StateType = std::pair<Grid<GameEntity>, Grid<Color>>;

StateType getState(const ALEScreen& screen);
Color getGoalColor(const ALEScreen& screen);
}
