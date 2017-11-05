#pragma once

#include <string>

#include "agent.h"
#include "learner.h"
#include "state-encoding.h"
#include "exploration-policy.h"

namespace Qbert {

// A class that implements a standard monolithic learning agent.
class MonolithicAgent : public Agent
{
    Learner learner;

public:
    // Contructs an agent with a reference to the current ALE instance, the
    // given name, the given state encoding function, and the given
    // exploration policy.
    MonolithicAgent(
        ALEInterface& ale,
        const std::string& name,
        StateEncoding encodeState,
        ExplorationPolicy explore);

    virtual ~MonolithicAgent() = default;

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
