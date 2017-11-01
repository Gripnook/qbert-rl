#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <functional>

#include <ale/ale_interface.hpp>

#include "image-processor.h"

namespace Qbert {

class Learner
{
    std::string name;
    std::function<int(const StateType&, int, int, Color, Color, int)>
        encodeState;

    std::unordered_map<int, std::array<float, 4>> utilities;
    std::unordered_map<int, std::array<int, 4>> visited;
    int currentState{-1}, lastState{-1};
    Action currentAction{Action::PLAYER_A_NOOP},
        lastAction{Action::PLAYER_A_NOOP};

    float randomActionCount{0};
    float totalActionCount{0};
    bool isRandomAction{true};

public:
    Learner(
        std::string name,
        std::function<int(const StateType&, int, int, Color, Color, int)>
            encodeState);

    void update(
        std::pair<int, int> position,
        const StateType& state,
        const Action& actionPerformed,
        float reward,
        Color startColor,
        Color goalColor,
        int level);
    void correctUpdate(float reward);

    Action getAction(
        std::pair<int, int> position,
        const StateType& state,
        Color startColor,
        Color goalColor,
        int level);

    void reset();

    float getRandomActionCount();
    float getTotalActionCount();
    float getRandomFraction();

private:
    std::vector<Action>
        getActions(std::pair<int, int> position, const StateType& state);
    static int actionToIndex(const Action& action);

    void loadFromFile();
    void saveToFile();
    void commitFile();
};
}
