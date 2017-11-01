#include "state.h"

#include "game-entity.h"

namespace Qbert {

bool hasEnemiesNearby(const StateType& state, int x, int y);

bool checkDangerousEnemy(const StateType& state, int x, int y);
bool checkDisc(const StateType& state, int x, int y);
bool checkVoid(const StateType& state, int x, int y);

int encodeColor(
    const StateType& state, int x, int y, Color startColor, Color goalColor);
int encodeCount(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor);

int countColor(
    const StateType& state, int x0, int x1, int y0, int y1, Color color);
int countIntermediateColor(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor);
bool checkColor(const StateType& state, int x, int y, Color color);

int encode(
    const StateType& state, int x, int y, Color startColor, Color goalColor)
{
    int result = 0;
    if (hasEnemiesNearby(state, x, y))
    {
        if (checkDangerousEnemy(state, x - 2, y))
            result |= 1 << 1;
        if (checkDangerousEnemy(state, x - 1, y - 1))
            result |= 1 << 2;
        if (checkDangerousEnemy(state, x - 1, y))
            result |= 1 << 3;
        if (checkDangerousEnemy(state, x - 1, y + 1))
            result |= 1 << 4;
        if (checkDangerousEnemy(state, x, y - 2))
            result |= 1 << 5;
        if (checkDangerousEnemy(state, x, y - 1))
            result |= 1 << 6;
        if (checkDangerousEnemy(state, x, y + 1))
            result |= 1 << 7;
        if (checkDangerousEnemy(state, x, y + 2))
            result |= 1 << 8;
        if (checkDangerousEnemy(state, x + 1, y - 1))
            result |= 1 << 9;
        if (checkDangerousEnemy(state, x + 1, y))
            result |= 1 << 10;
        if (checkDangerousEnemy(state, x + 1, y + 1))
            result |= 1 << 11;
        if (checkDangerousEnemy(state, x + 2, y))
            result |= 1 << 12;

        if (checkDisc(state, x - 2, y))
            result |= 1 << 13;
        if (checkDisc(state, x - 1, y - 1))
            result |= 1 << 14;
        if (checkDisc(state, x - 1, y))
            result |= 1 << 15;
        if (checkDisc(state, x - 1, y + 1))
            result |= 1 << 16;
        if (checkDisc(state, x, y - 2))
            result |= 1 << 17;
        if (checkDisc(state, x, y - 1))
            result |= 1 << 18;
        if (checkDisc(state, x + 1, y - 1))
            result |= 1 << 19;
    }
    else
    {
        // We haven't yet found out the colors, so we return a special state.
        if (startColor == 0 || goalColor == 0)
            return 0;

        result |= 1 << 0;
        result |= encodeColor(state, x, y, startColor, goalColor) << 1;
        result |= encodeColor(state, x - 1, y, startColor, goalColor) << 3;
        result |= encodeColor(state, x, y - 1, startColor, goalColor) << 5;
        result |= encodeColor(state, x + 1, y, startColor, goalColor) << 7;
        result |= encodeColor(state, x, y + 1, startColor, goalColor) << 9;
        if (checkDisc(state, x - 1, y))
            result |= encodeColor(state, 1, 1, startColor, goalColor) << 3;
        if (checkDisc(state, x, y - 1))
            result |= encodeColor(state, 1, 1, startColor, goalColor) << 5;

        result |= encodeCount(state, 1, 6, 1, y - 1, startColor, goalColor)
            << 11;
        result |= encodeCount(state, x + 1, 6, 1, 6, startColor, goalColor)
            << 14;
        result |= encodeCount(state, 1, x - 1, 1, 6, startColor, goalColor)
            << 17;
        result |= encodeCount(state, 1, 6, y + 1, 6, startColor, goalColor)
            << 20;
    }
    return result;
}

bool hasEnemiesNearby(const StateType& state, int x, int y)
{
    return checkDangerousEnemy(state, x - 2, y) ||
        checkDangerousEnemy(state, x - 1, y - 1) ||
        checkDangerousEnemy(state, x - 1, y) ||
        checkDangerousEnemy(state, x - 1, y + 1) ||
        checkDangerousEnemy(state, x, y - 2) ||
        checkDangerousEnemy(state, x, y - 1) ||
        checkDangerousEnemy(state, x, y + 1) ||
        checkDangerousEnemy(state, x, y + 2) ||
        checkDangerousEnemy(state, x + 1, y - 1) ||
        checkDangerousEnemy(state, x + 1, y) ||
        checkDangerousEnemy(state, x + 1, y + 1) ||
        checkDangerousEnemy(state, x + 2, y);
}

bool checkDangerousEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return isDangerousEnemy(state.first[x][y]);
}

bool checkDisc(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::Disc;
}

bool checkVoid(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return true;
    return state.first[x][y] == GameEntity::Void;
}

int encodeColor(
    const StateType& state, int x, int y, Color startColor, Color goalColor)
{
    if (checkColor(state, x, y, 0))
        return 0;
    else if (checkColor(state, x, y, startColor))
        return 1;
    else if (checkColor(state, x, y, goalColor))
        return 2;
    else
        return 3;
}

int encodeCount(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor)
{
    int result = 0;
    int startCount = countColor(state, x0, x1, y0, y1, startColor);
    if (startCount > 0)
        result |= 1 << 0;
    int goalCount = countColor(state, x0, x1, y0, y1, goalColor);
    if (goalCount > 0)
        result |= 1 << 1;
    int intermediateCount =
        countIntermediateColor(state, x0, x1, y0, y1, startColor, goalColor);
    if (intermediateCount > 0)
        result |= 1 << 2;
    return result;
}

int countColor(
    const StateType& state, int x0, int x1, int y0, int y1, Color color)
{
    int count = 0;
    for (int x = x0; x <= x1; ++x)
        for (int y = y0; y <= y1; ++y)
            if (checkColor(state, x, y, color))
                ++count;
    return count;
}

int countIntermediateColor(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor)
{
    int count = 0;
    for (int x = x0; x <= x1; ++x)
        for (int y = y0; y <= y1; ++y)
            if (!checkColor(state, x, y, 0) &&
                !checkColor(state, x, y, startColor) &&
                !checkColor(state, x, y, goalColor))
                ++count;
    return count;
}

bool checkColor(const StateType& state, int x, int y, Color color)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.second[x][y] == color;
}
}
