#pragma once

#include <functional>

#include "image-processor.h"

namespace Qbert {

using StateEncoding = std::function<
    int(const StateType& state,
        int x,
        int y,
        Color startColor,
        Color goalColor,
        int level)>;

// int encodeState(
//     const StateType& state,
//     int x,
//     int y,
//     Color startColor,
//     Color goalColor,
//     int level);

int encodeBlockState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

int encodeEnemyState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

int encodeEnemyStateWithSeparateCoily(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level);

// int encodeDangerousEnemyState(
//     const StateType& state,
//     int x,
//     int y,
//     Color startColor,
//     Color goalColor,
//     int level);

// int encodeGreenEnemyState(
//     const StateType& state,
//     int x,
//     int y,
//     Color startColor,
//     Color goalColor,
//     int level);

bool hasEnemiesNearby(const StateType& state, int x, int y);

// bool hasDangerousEnemiesNearby(const StateType& state, int x, int y);

// bool hasGreenEnemiesNearby(const StateType& state, int x, int y);
}
