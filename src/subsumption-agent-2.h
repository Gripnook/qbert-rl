#pragma once

#include "agent.h"
#include "learner.h"
#include "state.h"

namespace Qbert {

// A class that implements a learning agent that uses a subsumption architecture
// to divide the model into two learners. One of these learners is responsible
// for dealing with block puzzle solving, and the other with avoiding enemies.
class SubsumptionAgent2 : public Agent
{
    Learner blockSolver{"s2-block-solver", encodeBlockSolverState};
    Learner enemyAvoider{"s2-enemy-avoider", encodeEnemyAvoiderState};
    bool enemyAvoiderActionTaken{false};

public:
    // Contructs an agent with a reference to the current ALE instance.
    SubsumptionAgent2(ALEInterface& ale);

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
