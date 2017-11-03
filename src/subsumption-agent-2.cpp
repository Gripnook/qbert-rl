#include "subsumption-agent-2.h"

namespace Qbert {

SubsumptionAgent2::SubsumptionAgent2(ALEInterface& ale) : Agent{ale}
{
}

void SubsumptionAgent2::resetGame()
{
    Agent::resetGame();
    blockSolver.reset();
    enemyAvoider.reset();
    enemyAvoiderActionTaken = false;
}

float SubsumptionAgent2::getRandomFraction()
{
    float randomActionCount = blockSolver.getRandomActionCount() +
        enemyAvoider.getRandomActionCount();
    float totalActionCount =
        blockSolver.getTotalActionCount() + enemyAvoider.getTotalActionCount();
    return totalActionCount == 0 ? 0 : randomActionCount / totalActionCount;
}

void SubsumptionAgent2::update(
    std::pair<int, int> position,
    const StateType& state,
    const Action& actionPerformed,
    float reward,
    Color startColor,
    Color goalColor,
    int level)
{
    // The reward for a block changing color is +25, and the rewards involving
    // enemies are all multiples of 100, so we can divide the rewards
    // accurately by taking the positive modulus 100 for the block solver and
    // the rest for the enemy avoider.
    float blockSolverReward = fmod((fmod(reward, 100) + 100), 100);
    float enemyAvoiderReward = reward - blockSolverReward;
    blockSolver.update(
        position,
        state,
        actionPerformed,
        blockSolverReward,
        startColor,
        goalColor,
        level);
    enemyAvoider.update(
        position,
        state,
        actionPerformed,
        enemyAvoiderReward,
        startColor,
        goalColor,
        level);
    if (enemyAvoiderActionTaken)
        enemyAvoider.notifyActionTaken();
    else
        blockSolver.notifyActionTaken();
}

void SubsumptionAgent2::correctUpdate(float reward)
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

Action SubsumptionAgent2::getAction(
    std::pair<int, int> position,
    const StateType& state,
    Color startColor,
    Color goalColor,
    int level)
{
    // We use the presence of enemies to suppress the block solver and focus on
    // enemy avoidance.
    if (hasEnemiesNearby(state, position.first, position.second))
    {
        enemyAvoiderActionTaken = true;
        return enemyAvoider.getAction(
            position, state, startColor, goalColor, level);
    }
    else
    {
        enemyAvoiderActionTaken = false;
        return blockSolver.getAction(
            position, state, startColor, goalColor, level);
    }
}
}
