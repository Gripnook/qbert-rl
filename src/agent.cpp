#include "agent.h"

#include "game-entity.h"

namespace Qbert {

Agent::Agent(ALEInterface& ale) : ale{ale}
{
}

void Agent::update()
{
    auto screen = ale.getScreen();
    update(getState(screen), screen);

    float currentReward = ale.act(action);
    reward += currentReward;
    score += currentReward;
    highScore = std::max(highScore, score);
    if (ale.lives() < lives)
        reward -= 1000;
    lives = ale.lives();
}

void Agent::update(const StateType& state, const ALEScreen& screen)
{
    action = Action::PLAYER_A_NOOP;
    positionTracker = getPlayerPosition(state);

    auto ram = ale.getRAM();
    if (ram.get(0) == 0 && (ram.get(ram.size() - 1) & 0x01) == 1)
    {
        updateColors(state, screen, reward);
        if (levelUp)
        {
            reward = 0;
        }
        else
        {
            action = learner.getAction(
                positionTracker, state, startColor, goalColor);
            if (positionTracker != playerPosition)
            {
                playerPosition = positionTracker;
                learner.update(
                    playerPosition, state, reward, startColor, goalColor);
                reward = 0;
            }
            else
            {
                learner.correctUpdate(reward);
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
        learner.correctUpdate(-3000);
        levelUp = false;
    }

    Color color = getGoalColor(screen);
    if (goalColor == 0 && color != 0)
        goalColor = color;

    if (reward == 100)
    {
        ++levelUpCounter;
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
    learner.reset();
    startColor = 0;
    goalColor = 0;
    levelUpCounter = 0;
    levelUp = true;
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
