#include "agent.h"

#include "game-entity.h"

namespace Qbert {

Agent::Agent(ALEInterface& ale) : ale{ale}
{
}

void Agent::update()
{
    ale.getScreenRGB(rgb);
    Screen screen{rgb,
                  static_cast<int>(ale.getScreen().width()),
                  static_cast<int>(ale.getScreen().height())};

    Color color = getGoal(screen);
    if (color != 0)
        goal = color;

    float currentReward = ale.act(action);
    reward += currentReward;
    score += currentReward;
    highScore = std::max(highScore, score);
    if (ale.lives() < lives)
        reward -= 1000;
    lives = ale.lives();

    update(getState(screen));
}

void Agent::update(const StateType& state)
{
    auto position = getPlayerPosition(state);
    if (position != playerPosition)
    {
        playerPosition = position;
        action = learner.update(playerPosition, state, reward, goal);
        reward = 0;
    }
}

std::pair<int, int> Agent::getPlayerPosition(const StateType& state)
{
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if (state.first[i][j] == GameEntity::Qbert)
                return {i, j};
    return playerPosition;
}

void Agent::resetGame()
{
    learner.reset();
    goal = 0;
    lives = 0;
    reward = 0;
    score = 0;
    action = Action::PLAYER_A_NOOP;
    playerPosition = {0, 0};
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
