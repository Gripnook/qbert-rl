#pragma once

#include <vector>
#include <utility>

#include <ale/ale_interface.hpp>

#include "image-processor.h"
#include "learner.h"

namespace Qbert {

class Agent
{
    ALEInterface& ale;
    Learner learner;

    Color startColor{0}, goalColor{0};
    int levelUpCounter{0};
    bool levelUp{true};

    int lives{0};
    float reward{0};
    float score{0};
    float highScore{0};
    Action action{Action::PLAYER_A_NOOP};
    std::pair<int, int> playerPosition{0, 0};
    std::pair<int, int> positionTracker{0, 0};

public:
    Agent(ALEInterface& ale);

    void update();
    void resetGame();

    float getScore();
    float getHighScore();

    float getRandomFraction();

private:
    void update(const StateType& state, const ALEScreen& screen);
    void updateColors(
        const StateType& state, const ALEScreen& screen, float reward);
    std::pair<int, int> getPlayerPosition(const StateType& state);
};
}
