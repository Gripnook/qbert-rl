#include "monolithic-agent.h"

namespace Qbert {

MonolithicAgent::MonolithicAgent(
    ALEInterface& ale,
    const std::string& name,
    StateEncoding encodeState,
    ExplorationPolicy explore)
    : Agent{ale}, learner{name + "-learner", encodeState, explore}
{
}

void MonolithicAgent::resetGame()
{
    Agent::resetGame();
    learner.reset();
}

float MonolithicAgent::getRandomFraction()
{
    float randomActionCount = learner.getRandomActionCount();
    float totalActionCount = learner.getTotalActionCount();
    return totalActionCount == 0 ? 0 : randomActionCount / totalActionCount;
}

void MonolithicAgent::update(
    std::pair<int, int> position,
    const StateType& state,
    const Action& actionPerformed,
    float reward,
    Color startColor,
    Color goalColor,
    int level)
{
    learner.update(
        position, state, actionPerformed, reward, startColor, goalColor, level);
    learner.notifyActionTaken();
}

void MonolithicAgent::correctUpdate(float reward)
{
    learner.correctUpdate(reward);
}

Action MonolithicAgent::getAction(
    std::pair<int, int> position,
    const StateType& state,
    Color startColor,
    Color goalColor,
    int level)
{
    return learner.getAction(position, state, startColor, goalColor, level);
}
}
