#include "state.h"

#include "game-entity.h"

namespace Qbert {

bool checkPurpleEnemy(const StateType& state, int x, int y);

int encode(const StateType& state, int x, int y, Color goal)
{
    int result = 0;
    if (goal == 0)
        goal = state.second[x][y];

    if (state.second[x][y] == goal)
        result |= 1 << 0;
    if (state.second[x - 1][y] == goal)
        result |= 1 << 1;
    if (state.second[x + 1][y] == goal)
        result |= 1 << 2;
    if (state.second[x][y - 1] == goal)
        result |= 1 << 3;
    if (state.second[x][y + 1] == goal)
        result |= 1 << 4;

    if (state.first[x - 1][y] == GameEntity::Disc)
    {
        result |= 1 << 5;
        if (state.second[1][1] == goal)
            result |= 1 << 1;
    }
    if (state.first[x][y - 1] == GameEntity::Disc)
    {
        result |= 1 << 6;
        if (state.second[1][1] == goal)
            result |= 1 << 3;
    }

    if (checkPurpleEnemy(state, x - 2, y))
        result |= 1 << 7;
    if (checkPurpleEnemy(state, x - 1, y - 1))
        result |= 1 << 8;
    if (checkPurpleEnemy(state, x - 1, y))
        result |= 1 << 9;
    if (checkPurpleEnemy(state, x - 1, y + 1))
        result |= 1 << 10;
    if (checkPurpleEnemy(state, x, y - 2))
        result |= 1 << 11;
    if (checkPurpleEnemy(state, x, y - 1))
        result |= 1 << 12;
    if (checkPurpleEnemy(state, x, y + 1))
        result |= 1 << 13;
    if (checkPurpleEnemy(state, x, y + 2))
        result |= 1 << 14;
    if (checkPurpleEnemy(state, x + 1, y - 1))
        result |= 1 << 15;
    if (checkPurpleEnemy(state, x + 1, y))
        result |= 1 << 16;
    if (checkPurpleEnemy(state, x + 1, y + 1))
        result |= 1 << 17;
    if (checkPurpleEnemy(state, x + 2, y))
        result |= 1 << 18;

    return result;
}

bool checkPurpleEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::PurpleEnemy;
}
}
