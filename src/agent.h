#pragma once

#include <vector>
#include <utility>

#include <ale/ale_interface.hpp>

#include "image-processor.h"
#include "screen.h"
#include "learner.h"

namespace Qbert {

class Agent
{
    ALEInterface& ale;
    Learner learner;

    std::vector<unsigned char> rgb;
    Color goal{0};

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

private:
    void update(const StateType& state);
    std::pair<int, int> getPlayerPosition(const StateType& state);
};
}
