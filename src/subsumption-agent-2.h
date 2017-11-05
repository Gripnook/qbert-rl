#pragma once

#include <string>

#include "agent.h"
#include "learner.h"
#include "state-encoding.h"
#include "exploration-policy.h"

namespace Qbert {

// A function that defines when to suppress the block puzzle solver.
using SubsumptionSupression =
    std::function<bool(const StateType& state, int x, int y)>;

// A class that implements a learning agent that uses a subsumption architecture
// to divide the model into two learners. One of these learners is responsible
// for dealing with block puzzle solving, and the other with avoiding enemies.
class SubsumptionAgent2 : public Agent
{
    Learner blockSolver;
    Learner enemyAvoider;
    bool enemyAvoiderActionTaken{false};
    SubsumptionSupression suppress;

public:
    // Contructs an agent with a reference to the current ALE instance, the
    // given name, the given state encoding functions, the given suppression
    // function, and the given exploration policy.
    SubsumptionAgent2(
        ALEInterface& ale,
        const std::string& name,
        StateEncoding encodeBlockState,
        StateEncoding encodeEnemyState,
        SubsumptionSupression suppress,
        ExplorationPolicy explore);

    virtual ~SubsumptionAgent2() = default;

    // Resets the agent after a game over.
    virtual void resetGame() override;

    // Returns the fraction of random actions taken.
    virtual float getRandomFraction() override;

private:
    // Assigns the given reward to the learners.
    virtual void update(
        std::pair<int, int> position,
        const StateType& state,
        const Action& actionPerformed,
        float reward,
        Color startColor,
        Color goalColor,
        int level) override;

    // Assigns an additional reward to the learners without updating the state.
    virtual void correctUpdate(float reward) override;

    // Gets the best action from the learners.
    virtual Action getAction(
        std::pair<int, int> position,
        const StateType& state,
        Color startColor,
        Color goalColor,
        int level) override;
};
}
