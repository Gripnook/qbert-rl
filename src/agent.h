#pragma once

#include <vector>
#include <utility>

#include <ale/ale_interface.hpp>

#include "image-processor.h"
#include "learner.h"
#include "state.h"

namespace Qbert {

class Agent
{
    ALEInterface& ale;
    Learner blockSolver{"block-solver", encodeBlockSolverState};
    Learner enemyAvoider{"enemy-avoider", encodeEnemyAvoiderState};

    Color startColor{0}, goalColor{0};
    int levelUpCounter{0};
    bool levelUp{true};
    int level{-1};

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

    void update(const StateType& state, float reward);
    void correctUpdate(float reward);

    Action getAction(const StateType& state);

    std::pair<int, int> getPlayerPosition(const StateType& state);

    void fixState(StateType& state);
};
}
