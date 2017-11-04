#pragma once

#include <functional>

#include "image-processor.h"

namespace Qbert {

// A function that defines how to encode a given state.
using StateEncoding = std::function<
    int(const StateType& state,
        int x,
        int y,
        Color startColor,
        Color goalColor,
        int level)>;

// A unified state encoding for game entities and blocks.
int encodeState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

// A separate state encoding that only looks at the block colors.
int encodeBlockState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

// A separate state encoding that only looks at the game entities.
int encodeEnemyState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

// A separate state encoding that only looks at the game entities. This function
// separates Coily from the other enemies in the encoding.
int encodeEnemyStateWithSeparateCoily(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

// Checks if any dangerous enemies are at a distance of 2 or less from the
// player, or if any green enemies are at a distance of 1 from the player.
bool hasEnemiesNearby(const StateType& state, int x, int y);

// Checks if Coily is at a distance of 2 or less from the player, or if any
// other dangerous enemies are at a distance of 1 from the player or could get
// there in a single move, or if any green enemies are at a distance of 1 from
// the player.
bool hasEnemiesNearbyWithSeparateCoily(const StateType& state, int x, int y);
}
