#include "agent.h"

#include <cmath>

#include "game-entity.h"

namespace Qbert {

Agent::Agent(ALEInterface& ale) : ale{ale}
{
}

void Agent::update()
{
    auto screen = ale.getScreen();
    auto state = getState(screen);
    fixState(state);
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
            action = getAction(state);
            if (positionTracker != playerPosition)
            {
                playerPosition = positionTracker;
                update(state, reward);
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

void Agent::update(const StateType& state, float reward)
{
    // The reward for a block changing color is +25, and the rewards involving
    // enemies are all multiples of 100, so we can divide the rewards
    // accurately by taking the positive modulus 100 for the block solver and
    // the rest for the enemy avoider.
    float blockSolverReward = fmod((fmod(reward, 100) + 100), 100);
    float enemyAvoiderReward = reward - blockSolverReward;
    blockSolver.update(
        playerPosition,
        state,
        action,
        blockSolverReward,
        startColor,
        goalColor,
        level);
    enemyAvoider.update(
        playerPosition,
        state,
        action,
        enemyAvoiderReward,
        startColor,
        goalColor,
        level);
    if (enemyAvoiderActionTaken)
        enemyAvoider.notifyActionTaken();
    else
        blockSolver.notifyActionTaken();
}

void Agent::correctUpdate(float reward)
{
    // The reward for a block changing color is +25, and the rewards involving
    // enemies are all multiples of 100, so we can divide the rewards
    // accurately by taking the positive modulus 100 for the block solver and
    // the rest for the enemy avoider.
    float blockSolverReward = fmod((fmod(reward, 100) + 100), 100);
    float enemyAvoiderReward = reward - blockSolverReward;
    blockSolver.correctUpdate(blockSolverReward);
    enemyAvoider.correctUpdate(enemyAvoiderReward);
}

Action Agent::getAction(const StateType& state)
{
    // We use the presence of enemies to suppress the block solver and focus on
    // enemy avoidance.
    if (hasEnemiesNearby(state, positionTracker.first, positionTracker.second))
    {
        enemyAvoiderActionTaken = true;
        return enemyAvoider.getAction(
            positionTracker, state, startColor, goalColor, level);
    }
    else
    {
        enemyAvoiderActionTaken = false;
        return blockSolver.getAction(
            positionTracker, state, startColor, goalColor, level);
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
    blockSolver.reset();
    enemyAvoider.reset();
    enemyAvoiderActionTaken = false;
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

float Agent::getRandomFraction()
{
    float randomActionCount = blockSolver.getRandomActionCount() +
        enemyAvoider.getRandomActionCount();
    float totalActionCount =
        blockSolver.getTotalActionCount() + enemyAvoider.getTotalActionCount();
    return totalActionCount == 0 ? 0 : randomActionCount / totalActionCount;
}

void Agent::fixState(StateType& state)
{
    auto ram = ale.getRAM();
    int xCoily = ram.get(0x27);
    int yCoily = ram.get(0x45);
    // Converts the diagonal position given in RAM to our coordinate system.
    int x = (xCoily - yCoily + 5) / 2 + 1;
    int y = (xCoily + yCoily - 5) / 2 + 1;
    if (x >= 0 && x < 8 && y >= 0 && y < 8 && state.second[x][y] != 0)
        state.first[x][y] = GameEntity::Coily;
}
}
