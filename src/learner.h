#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

#include <ale/ale_interface.hpp>

#include "feature-extractor.h"
#include "state-encoding.h"
#include "exploration-policy.h"

namespace Qbert {

// A class that implements the Q-learning algorithm.
class Learner
{
    const std::string name;
    const StateEncoding encodeState;
    const ExplorationPolicy explore;
    const bool allowNoop;
    const float alpha, gamma;

    std::unordered_map<int, std::array<float, 5>> utilities;
    std::unordered_map<int, std::array<int, 5>> visited;
    int currentState{-1}, lastState{-1};
    Action currentAction{Action::PLAYER_A_NOOP},
        lastAction{Action::PLAYER_A_NOOP};

    float randomActionCount{0};
    float totalActionCount{0};
    bool isRandomAction{true};

public:
    // Constructs a learner with the given name, state encoding function,
    // exploration policy, and learning parameters.
    Learner(
        std::string name,
        StateEncoding encodeState,
        ExplorationPolicy explore,
        bool allowNoop = false,
        float alpha = 0.10f,
        float gamma = 0.90f);

    // Updates the state of the learner and assigns the given reward to the
    // last state transition.
    void update(
        std::pair<int, int> position,
        const StateType& state,
        const Action& actionPerformed,
        float reward,
        Color startColor,
        Color goalColor,
        int level);

    // Assigns an additional reward to the last state transition.
    void correctUpdate(float reward);

    // Returns the best action to take from the point of view of this learner.
    Action getAction(
        std::pair<int, int> position,
        const StateType& state,
        Color startColor,
        Color goalColor,
        int level);

    // Notifies this learner that its suggested action was taken.
    void notifyActionTaken();

    // Resets the learner after a game over.
    void reset();

    float getRandomActionCount();
    float getTotalActionCount();
    float getRandomFraction();

private:
    // Returns the valid actions for the given state (the ones that don't result
    // in guaranteed insta-death).
    std::vector<Action>
        getActions(std::pair<int, int> position, const StateType& state);

    // Maps the actions to their index in the utility arrays.
    static int actionToIndex(const Action& action);

    // Loads the utilities from a file.
    void loadFromFile();

    // Saves the utilities to a file.
    void saveToFile();

    // Commits the saved file by renaming a temporary copy. This allows for
    // less chance of corruption if the program is interrupted.
    void commitFile();
};
}
