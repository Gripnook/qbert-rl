#include "state-encoding.h"

#include "game-entity.h"

namespace Qbert {

// Encodes dangerous enemies at a distance of 2 or less from the player
// in an 8-bit encoding.
int encodeDangerousEnemies(const StateType& state, int x, int y);

// Encodes Coily's presence at a distance of 2 or less from the player
// in a 4-bit encoding.
int encodeCoily(const StateType& state, int x, int y);

// Encodes Coily's presence at a distance of 3 or less from the player
// in a 5-bit encoding.
int encodeCoilyV2(const StateType& state, int x, int y);

// Encodes dangerous balls that are at a distance of 1 from the player, or could
// get there in a single move, in a 7-bit encoding.
int encodeDangerousBalls(const StateType& state, int x, int y);

// Encodes green enemies at a distance of 1 from the player
// in a 5-bit encoding.
int encodeGreenEnemies(const StateType& state, int x, int y);

// Encodes discs at a distance of 2 or less from the player
// in a 3-bit encoding.
int encodeDiscs(const StateType& state, int x, int y);

// Checks if there is a dangerous enemy at position (x, y).
bool checkDangerousEnemy(const StateType& state, int x, int y);

// Checks if Coily is at position (x, y).
bool checkCoily(const StateType& state, int x, int y);

// Checks if there is a dangerous ball at position (x, y).
bool checkDangerousBalls(const StateType& state, int x, int y);

// Checks if there is a green enemy at position (x, y).
bool checkGreenEnemy(const StateType& state, int x, int y);

// Checks if there is a disc at position (x, y).
bool checkDisc(const StateType& state, int x, int y);

// Checks if there is a void at position (x, y).
bool checkVoid(const StateType& state, int x, int y);

// Encodes the color of the block at position (x, y) in a 2-bit encoding.
int encodeColor(
    const StateType& state, int x, int y, Color startColor, Color goalColor);

// Encodes a count of the blocks in the rectangle defined by
// (x0, y0) and (x1, y1) in a 3-bit encoding.
int encodeCount(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor);

// Counts the number of blocks with the given color in the rectangle
// defined by (x0, y0) and (x1, y1).
int countColor(
    const StateType& state, int x0, int x1, int y0, int y1, Color color);

// Counts the number of blocks with an intermediate color in the rectangle
// defined by (x0, y0) and (x1, y1).
int countIntermediateColor(
    const StateType& state,
    int x0,
    int x1,
    int y0,
    int y1,
    Color startColor,
    Color goalColor);

// Checks if the block at position (x, y) has the given color.
bool checkColor(const StateType& state, int x, int y, Color color);

// Counts the number of moves the player can take in the given position.
// Returns a 4-bit encoding.
int countMoves(const StateType& state, int x, int y);

int encodeState(
    const StateType& state,
    int x,
    int y,
    Color startColor,
    Color goalColor,
    int /*level*/)
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

    result |= encodeDangerousEnemies(state, x, y) << 11;
    result |= encodeGreenEnemies(state, x, y) << 19;
    result |= encodeDiscs(state, x, y) << 24;

    return result;
}

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

int encodeEnemyStateWithSeparateCoilyV2(
    const StateType& state,
    int x,
    int y,
    Color /*startColor*/,
    Color /*goalColor*/,
    int /*level*/)
{
    int result = 0;

    result |= encodeCoilyV2(state, x, y) << 0;
    result |= encodeDangerousBalls(state, x, y) << 5;
    result |= encodeGreenEnemies(state, x, y) << 12;
    result |= encodeDiscs(state, x, y) << 17;

    result |= countMoves(state, x, y) << 20;
    result |= ((x + 1) >> 1) << 24;
    result |= ((y + 1) >> 1) << 26;

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
        checkDangerousEnemy(state, x + 2, y) ||
        checkGreenEnemy(state, x - 1, y) || checkGreenEnemy(state, x, y - 1) ||
        checkGreenEnemy(state, x + 1, y) || checkGreenEnemy(state, x, y + 1);
}

bool hasEnemiesNearbyWithSeparateCoily(const StateType& state, int x, int y)
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

bool hasEnemiesNearbyWithSeparateCoilyV2(const StateType& state, int x, int y)
{
    return checkCoily(state, x - 3, y) || checkCoily(state, x - 2, y - 1) ||
        checkCoily(state, x - 2, y) || checkCoily(state, x - 2, y + 1) ||
        checkCoily(state, x - 1, y - 2) || checkCoily(state, x - 1, y - 1) ||
        checkCoily(state, x - 1, y) || checkCoily(state, x - 1, y + 1) ||
        checkCoily(state, x - 1, y + 2) || checkCoily(state, x, y - 3) ||
        checkCoily(state, x, y - 2) || checkCoily(state, x, y - 1) ||
        checkCoily(state, x, y + 1) || checkCoily(state, x, y + 2) ||
        checkCoily(state, x, y + 3) || checkCoily(state, x + 1, y - 2) ||
        checkCoily(state, x + 1, y - 1) || checkCoily(state, x + 1, y) ||
        checkCoily(state, x + 1, y + 1) || checkCoily(state, x + 1, y + 2) ||
        checkCoily(state, x + 2, y - 1) || checkCoily(state, x + 2, y) ||
        checkCoily(state, x + 2, y + 1) || checkCoily(state, x + 3, y) ||
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

    // There can only be two dangerous enemies in the game at a time.
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

    // There can only be one instance of Coily in the game at a time.
    for (int i = 0; i < 12; ++i)
        if (((result >> i) & 0x01) != 0)
            return i + 1;
    return 0;
}

int encodeCoilyV2(const StateType& state, int x, int y)
{
    int result = 0;
    if (checkCoily(state, x - 3, y))
        result |= 1 << 0;
    if (checkCoily(state, x - 2, y - 1))
        result |= 1 << 1;
    if (checkCoily(state, x - 2, y))
        result |= 1 << 2;
    if (checkCoily(state, x - 2, y + 1))
        result |= 1 << 3;
    if (checkCoily(state, x - 1, y - 2))
        result |= 1 << 4;
    if (checkCoily(state, x - 1, y - 1))
        result |= 1 << 5;
    if (checkCoily(state, x - 1, y))
        result |= 1 << 6;
    if (checkCoily(state, x - 1, y + 1))
        result |= 1 << 7;
    if (checkCoily(state, x - 1, y + 2))
        result |= 1 << 8;
    if (checkCoily(state, x, y - 3))
        result |= 1 << 9;
    if (checkCoily(state, x, y - 2))
        result |= 1 << 10;
    if (checkCoily(state, x, y - 1))
        result |= 1 << 11;
    if (checkCoily(state, x, y + 1))
        result |= 1 << 12;
    if (checkCoily(state, x, y + 2))
        result |= 1 << 13;
    if (checkCoily(state, x, y + 3))
        result |= 1 << 14;
    if (checkCoily(state, x + 1, y - 2))
        result |= 1 << 15;
    if (checkCoily(state, x + 1, y - 1))
        result |= 1 << 16;
    if (checkCoily(state, x + 1, y))
        result |= 1 << 17;
    if (checkCoily(state, x + 1, y + 1))
        result |= 1 << 18;
    if (checkCoily(state, x + 1, y + 2))
        result |= 1 << 19;
    if (checkCoily(state, x + 2, y - 1))
        result |= 1 << 20;
    if (checkCoily(state, x + 2, y))
        result |= 1 << 21;
    if (checkCoily(state, x + 2, y + 1))
        result |= 1 << 22;
    if (checkCoily(state, x + 3, y))
        result |= 1 << 23;

    // There can only be one instance of Coily in the game at a time.
    for (int i = 0; i < 24; ++i)
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

    // There can only be two dangerous balls in the game at a time.
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

    // There can only be two green enemies in the game at a time.
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

    // There can only be one disc in the given positions at a time.
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
    return state.first[x][y] == GameEntity::PurpleBall ||
        state.first[x][y] == GameEntity::RedBall;
}

bool checkGreenEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::Sam ||
        state.first[x][y] == GameEntity::GreenBall;
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
