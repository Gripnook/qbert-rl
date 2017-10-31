#include "state.h"

#include "game-entity.h"

namespace Qbert {

bool hasEnemiesNearby(const StateType& state, int x, int y);

bool checkPurpleEnemy(const StateType& state, int x, int y);
bool checkDisc(const StateType& state, int x, int y);
bool checkVoid(const StateType& state, int x, int y);
bool checkGoal(const StateType& state, int x, int y, Color goal);

int encode(const StateType& state, int x, int y, Color goal)
{
    int result = 0;
    if (goal == 0)
        goal = state.second[x][y];

    if (hasEnemiesNearby(state, x, y))
    {
        result |= 1 << 0;

        if (checkPurpleEnemy(state, x - 2, y))
            result |= 1 << 1;
        if (checkPurpleEnemy(state, x - 1, y - 1))
            result |= 1 << 2;
        if (checkPurpleEnemy(state, x - 1, y))
            result |= 1 << 3;
        if (checkPurpleEnemy(state, x - 1, y + 1))
            result |= 1 << 4;
        if (checkPurpleEnemy(state, x, y - 2))
            result |= 1 << 5;
        if (checkPurpleEnemy(state, x, y - 1))
            result |= 1 << 6;
        if (checkPurpleEnemy(state, x, y + 1))
            result |= 1 << 7;
        if (checkPurpleEnemy(state, x, y + 2))
            result |= 1 << 8;
        if (checkPurpleEnemy(state, x + 1, y - 1))
            result |= 1 << 9;
        if (checkPurpleEnemy(state, x + 1, y))
            result |= 1 << 10;
        if (checkPurpleEnemy(state, x + 1, y + 1))
            result |= 1 << 11;
        if (checkPurpleEnemy(state, x + 2, y))
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
        if (checkDisc(state, x, y + 1))
            result |= 1 << 19;
        if (checkDisc(state, x, y + 2))
            result |= 1 << 20;
        if (checkDisc(state, x + 1, y - 1))
            result |= 1 << 21;
        if (checkDisc(state, x + 1, y))
            result |= 1 << 22;
        if (checkDisc(state, x + 1, y + 1))
            result |= 1 << 23;
        if (checkDisc(state, x + 2, y))
            result |= 1 << 24;
    }
    else
    {
        if (checkGoal(state, x - 1, y - 1, goal))
            result |= 1 << 1;
        else if (checkVoid(state, x - 1, y - 1))
            result |= 2 << 1;

        if (checkGoal(state, x - 1, y, goal))
            result |= 1 << 3;
        else if (checkVoid(state, x - 1, y))
            result |= 2 << 3;

        if (checkGoal(state, x - 1, y + 1, goal))
            result |= 1 << 5;
        else if (checkVoid(state, x - 1, y + 1))
            result |= 2 << 5;

        if (checkGoal(state, x, y - 1, goal))
            result |= 1 << 7;
        else if (checkVoid(state, x, y - 1))
            result |= 2 << 7;

        if (checkGoal(state, x, y, goal))
            result |= 1 << 9;
        else if (checkVoid(state, x, y))
            result |= 2 << 9;

        if (checkGoal(state, x, y + 1, goal))
            result |= 1 << 11;
        else if (checkVoid(state, x, y + 1))
            result |= 2 << 11;

        if (checkGoal(state, x + 1, y - 1, goal))
            result |= 1 << 13;
        else if (checkVoid(state, x + 1, y - 1))
            result |= 2 << 13;

        if (checkGoal(state, x + 1, y, goal))
            result |= 1 << 15;
        else if (checkVoid(state, x + 1, y))
            result |= 2 << 15;

        if (checkGoal(state, x + 1, y + 1, goal))
            result |= 1 << 17;
        else if (checkVoid(state, x + 1, y + 1))
            result |= 2 << 17;
    }

    return result;
}

bool hasEnemiesNearby(const StateType& state, int x, int y)
{
    return checkPurpleEnemy(state, x - 2, y) ||
        checkPurpleEnemy(state, x - 1, y - 1) ||
        checkPurpleEnemy(state, x - 1, y) ||
        checkPurpleEnemy(state, x - 1, y + 1) ||
        checkPurpleEnemy(state, x, y - 2) ||
        checkPurpleEnemy(state, x, y - 1) ||
        checkPurpleEnemy(state, x, y + 1) ||
        checkPurpleEnemy(state, x, y + 2) ||
        checkPurpleEnemy(state, x + 1, y - 1) ||
        checkPurpleEnemy(state, x + 1, y) ||
        checkPurpleEnemy(state, x + 1, y + 1) ||
        checkPurpleEnemy(state, x + 2, y);
}

bool checkPurpleEnemy(const StateType& state, int x, int y)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.first[x][y] == GameEntity::PurpleEnemy;
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

bool checkGoal(const StateType& state, int x, int y, Color goal)
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8)
        return false;
    return state.second[x][y] == goal;
}
}
