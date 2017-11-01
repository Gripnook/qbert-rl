#pragma once

#include "image-processor.h"

namespace Qbert {

int encodeBlockSolverState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);
int encodeEnemyAvoiderState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

bool hasEnemiesNearby(const StateType& state, int x, int y);
}
