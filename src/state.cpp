#include "state.h"

#include "game-entity.h"

namespace Qbert {

int encodeDangerousEnemies(const StateType& state, int x, int y);
int encodeCoily(const StateType& state, int x, int y);
int encodeDangerousBalls(const StateType& state, int x, int y);
int encodeGreenEnemies(const StateType& state, int x, int y);
int encodeDiscs(const StateType& state, int x, int y);

bool checkDangerousEnemy(const StateType& state, int x, int y);
bool checkCoily(const StateType& state, int x, int y);
bool checkDangerousBalls(const StateType& state, int x, int y);
bool checkGreenEnemy(const StateType& state, int x, int y);
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

int countMoves(const StateType& state, int x, int y);

int encodeBlockState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int level)
{
    int result = 0;

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

    result |= encodeCount(state, 1, 6, 1, y - 1, startColor, goalColor) << 11;
    result |= encodeCount(state, x + 1, 6, 1, 6, startColor, goalColor) << 14;
    result |= encodeCount(state, 1, x - 1, 1, 6, startColor, goalColor) << 17;
    result |= encodeCount(state, 1, 6, y + 1, 6, startColor, goalColor) << 20;

    result |= std::min(level / 4, 4) << 23;

    return result;
}

int encodeEnemyState(
    const StateType& state,
    int x,
    int y,
    Color /*startColor*/,
    Color /*goalColor*/,
    int /*level*/)
{
    int result = 0;

    result |= encodeDangerousEnemies(state, x, y) << 0;
    result |= encodeGreenEnemies(state, x, y) << 8;
    result |= encodeDiscs(state, x, y) << 13;

    result |= countMoves(state, x, y) << 16;
    result |= ((x + 1) >> 1) << 20;
    result |= ((y + 1) >> 1) << 22;

    return result;
}

int encodeEnemyStateWithSeparateCoily(
    const StateType& state,
    int x,
    int y,
    Color /*startColor*/,
    Color /*goalColor*/,
    int /*level*/)
{
    int result = 0;

    result |= encodeCoily(state, x, y) << 0;
    result |= encodeDangerousBalls(state, x, y) << 4;
    result |= encodeGreenEnemies(state, x, y) << 11;
    result |= encodeDiscs(state, x, y) << 16;

    result |= countMoves(state, x, y) << 19;
    result |= ((x + 1) >> 1) << 23;
    result |= ((y + 1) >> 1) << 25;

    return result;
}

bool hasEnemiesNearby(const StateType& state, int x, int y)
{
    return checkCoily(state, x - 2, y) || checkCoily(state, x - 1, y - 1) ||
        checkCoily(state, x - 1, y) || checkCoily(state, x - 1, y + 1) ||
        checkCoily(state, x, y - 2) || checkCoily(state, x, y - 1) ||
        checkCoily(state, x, y + 1) || checkCoily(state, x, y + 2) ||
        checkCoily(state, x + 1, y - 1) || checkCoily(state, x + 1, y) ||
        checkCoily(state, x + 1, y + 1) || checkCoily(state, x + 2, y) ||
        checkDangerousBalls(state, x - 2, y) ||
        checkDangerousBalls(state, x - 1, y - 1) ||
        checkDangerousBalls(state, x - 1, y) ||
        checkDangerousBalls(state, x - 1, y + 1) ||
        checkDangerousBalls(state, x, y - 2) ||
        checkDangerousBalls(state, x, y - 1) ||
        checkDangerousBalls(state, x, y + 1) ||
        checkDangerousBalls(state, x + 1, y - 1) ||
        checkDangerousBalls(state, x + 1, y) ||
        checkGreenEnemy(state, x - 1, y) || checkGreenEnemy(state, x, y - 1) ||
        checkGreenEnemy(state, x + 1, y) || checkGreenEnemy(state, x, y + 1);
}

int encodeDangerousEnemies(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkDangerousEnemy(state, x - 2, y))
        result |= 1 << 0;
    if (checkDangerousEnemy(state, x - 1, y - 1))
        result |= 1 << 1;
    if (checkDangerousEnemy(state, x - 1, y))
        result |= 1 << 2;
    if (checkDangerousEnemy(state, x - 1, y + 1))
        result |= 1 << 3;
    if (checkDangerousEnemy(state, x, y - 2))
        result |= 1 << 4;
    if (checkDangerousEnemy(state, x, y - 1))
        result |= 1 << 5;
    if (checkDangerousEnemy(state, x, y + 1))
        result |= 1 << 6;
    if (checkDangerousEnemy(state, x, y + 2))
        result |= 1 << 7;
    if (checkDangerousEnemy(state, x + 1, y - 1))
        result |= 1 << 8;
    if (checkDangerousEnemy(state, x + 1, y))
        result |= 1 << 9;
    if (checkDangerousEnemy(state, x + 1, y + 1))
        result |= 1 << 10;
    if (checkDangerousEnemy(state, x + 2, y))
        result |= 1 << 11;

    int first = 0;
    int second = 0;
    for (int i = 0; i < 12; ++i)
    {
        if (((result >> i) & 0x01) != 0)
        {
            if (first == 0)
                first = i + 1;
            else
                second = i;
        }
    }
    return first * 12 + second;
}

int encodeCoily(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkCoily(state, x - 2, y))
        result |= 1 << 0;
    if (checkCoily(state, x - 1, y - 1))
        result |= 1 << 1;
    if (checkCoily(state, x - 1, y))
        result |= 1 << 2;
    if (checkCoily(state, x - 1, y + 1))
        result |= 1 << 3;
    if (checkCoily(state, x, y - 2))
        result |= 1 << 4;
    if (checkCoily(state, x, y - 1))
        result |= 1 << 5;
    if (checkCoily(state, x, y + 1))
        result |= 1 << 6;
    if (checkCoily(state, x, y + 2))
        result |= 1 << 7;
    if (checkCoily(state, x + 1, y - 1))
        result |= 1 << 8;
    if (checkCoily(state, x + 1, y))
        result |= 1 << 9;
    if (checkCoily(state, x + 1, y + 1))
        result |= 1 << 10;
    if (checkCoily(state, x + 2, y))
        result |= 1 << 11;

    for (int i = 0; i < 12; ++i)
        if (((result >> i) & 0x01) != 0)
            return i + 1;
    return 0;
}

int encodeDangerousBalls(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkDangerousBalls(state, x - 2, y))
        result |= 1 << 0;
    if (checkDangerousBalls(state, x - 1, y - 1))
        result |= 1 << 1;
    if (checkDangerousBalls(state, x - 1, y))
        result |= 1 << 2;
    if (checkDangerousBalls(state, x - 1, y + 1))
        result |= 1 << 3;
    if (checkDangerousBalls(state, x, y - 2))
        result |= 1 << 4;
    if (checkDangerousBalls(state, x, y - 1))
        result |= 1 << 5;
    if (checkDangerousBalls(state, x, y + 1))
        result |= 1 << 6;
    if (checkDangerousBalls(state, x + 1, y - 1))
        result |= 1 << 7;
    if (checkDangerousBalls(state, x + 1, y))
        result |= 1 << 8;

    int first = 0;
    int second = 0;
    for (int i = 0; i < 9; ++i)
    {
        if (((result >> i) & 0x01) != 0)
        {
            if (first == 0)
                first = i + 1;
            else
                second = i;
        }
    }
    return first * 9 + second;
}

int encodeGreenEnemies(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkGreenEnemy(state, x - 1, y))
        result |= 1 << 0;
    if (checkGreenEnemy(state, x, y - 1))
        result |= 1 << 1;
    if (checkGreenEnemy(state, x, y + 1))
        result |= 1 << 2;
    if (checkGreenEnemy(state, x + 1, y))
        result |= 1 << 3;

    int first = 0;
    int second = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (((result >> i) & 0x01) != 0)
        {
            if (first == 0)
                first = i + 1;
            else
                second = i;
        }
    }
    return first * 4 + second;
}

int encodeDiscs(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkDisc(state, x - 2, y))
        result |= 1 << 0;
    if (checkDisc(state, x - 1, y - 1))
        result |= 1 << 1;
    if (checkDisc(state, x - 1, y))
        result |= 1 << 2;
    if (checkDisc(state, x - 1, y + 1))
        result |= 1 << 3;
    if (checkDisc(state, x, y - 2))
        result |= 1 << 4;
    if (checkDisc(state, x, y - 1))
        result |= 1 << 5;
    if (checkDisc(state, x + 1, y - 1))
        result |= 1 << 6;

    for (int i = 0; i < 7; ++i)
        if (((result >> i) & 0x01) != 0)
            return i + 1;
    return 0;
}

bool checkDangerousEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return isDangerousEnemy(state.first[x][y]);
}

bool checkCoily(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::Coily;
}

bool checkDangerousBalls(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::PurpleEnemy ||
        state.first[x][y] == GameEntity::RedEnemy;
}

bool checkGreenEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::GreenEnemy;
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
            if (!checkVoid(state, x, y) && !checkColor(state, x, y, 0) &&
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

int countMoves(const StateType& state, int x, int y)
{
    int result = 0;
    if (!checkVoid(state, x - 1, y))
        result |= 1 << 0;
    if (!checkVoid(state, x, y - 1))
        result |= 1 << 1;
    if (!checkVoid(state, x + 1, y))
        result |= 1 << 2;
    if (!checkVoid(state, x, y + 1))
        result |= 1 << 3;
    return result;
}
}
