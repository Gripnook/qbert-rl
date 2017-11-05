#include "agent.h"

#include <cmath>

#include "game-entity.h"

namespace Qbert {

Agent::Agent(ALEInterface& ale) : ale{ale}
{
}

void Agent::updateState()
{
    auto screen = ale.getScreen();
    auto state = getState(ale);
    update(state, screen);

    float currentReward = ale.act(action);
    reward += currentReward;
    score += currentReward;
    highScore = std::max(highScore, score);
    if (ale.lives() < lives)
        reward -= 1000; // We assign an arbitrary negative reward to deaths.
    lives = ale.lives();
}

void Agent::update(const StateType& state, const ALEScreen& screen)
{
    action = Action::PLAYER_A_NOOP;
    positionTracker = getPlayerPosition(state);

    // The combination of the first byte in RAM being 0 and the last bit in RAM
    // being 1 is a good signal for the game accepting actions from the player.
    auto ram = ale.getRAM();
    if (ram.get(0x00) == 0 && (ram.get(0x7F) & 0x01) == 1)
    {
        updateColors(state, screen, reward);
        if (levelUp)
        {
            reward = 0;
        }
        else
        {
            action =
                getAction(positionTracker, state, startColor, goalColor, level);
            if (positionTracker != playerPosition)
            {
                playerPosition = positionTracker;
                update(
                    playerPosition,
                    state,
                    action,
                    reward,
                    startColor,
                    goalColor,
                    level);
                reward = 0;
            }
            else
            {
                correctUpdate(reward);
                reward = 0;
            }
        }
    }
}

void Agent::updateColors(
    const StateType& state, const ALEScreen& screen, float reward)
{
    if (levelUp && positionTracker == std::make_pair(1, 1))
    {
        startColor = state.second[1][1];
        // We revert the 30 +100 rewards given for completing the level.
        for (int i = 0; i < 30; ++i)
            correctUpdate(-100);
        levelUp = false;
        ++level;
    }

    Color color = getGoalColor(screen);
    if (goalColor == 0 && color != 0)
        goalColor = color;

    if (reward == 100)
    {
        ++levelUpCounter;
        // A set of 31 +100 rewards in a row signals a level up.
        if (levelUpCounter == 31)
        {
            levelUp = true;
            startColor = 0;
            goalColor = 0;
        }
    }
    else
    {
        levelUpCounter = 0;
    }
}

std::pair<int, int> Agent::getPlayerPosition(const StateType& state)
{
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (state.first[i][j] == GameEntity::Qbert)
                return {i, j};
    return positionTracker;
}

void Agent::resetGame()
{
    startColor = 0;
    goalColor = 0;
    levelUpCounter = 0;
    levelUp = true;
    level = -1;
    lives = 0;
    reward = 0;
    score = 0;
    action = Action::PLAYER_A_NOOP;
    playerPosition = {0, 0};
    positionTracker = {0, 0};
}

float Agent::getScore()
{
    return score;
}

float Agent::getHighScore()
{
    return highScore;
}
}
