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

// Processes the image on the screen and the RAM to obtain the locations of
// Qbert, Coily, and the other entities, as well as the colors of all the blocks
// in the game.
StateType getState(ALEInterface& ale);

// Tries to get the current goal color from the game score on the screen. Note
// that this method can fail and return 0 if the score is currently not being
// displayed.
Color getGoalColor(const ALEScreen& screen);
}
