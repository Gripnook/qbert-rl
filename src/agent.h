#pragma once

#include <vector>
#include <utility>

#include <ale/ale_interface.hpp>

#include "image-processor.h"
#include "learner.h"
#include "state.h"

namespace Qbert {

// A class that implements a learning agent that uses a subsumption architecture
// to divide the model into two learners. One of these learners is responsible
// for dealing with block puzzle solving, and the other with avoiding enemies.
class Agent
{
    ALEInterface& ale;
    Learner blockSolver{"block-solver", encodeBlockSolverState};
    Learner enemyAvoider{"enemy-avoider", encodeEnemyAvoiderState};
    bool enemyAvoiderActionTaken{false};

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
    // Contructs an agent with a reference to the current ALE instance.
    Agent(ALEInterface& ale);

    // Updates the state of the game. This should be called every frame.
    void update();

    // Resets the agent after a game over.
    void resetGame();

    // Returns the current score for this game.
    float getScore();

    // Returns the best score seen so far.
    float getHighScore();

    // Returns the fraction of random actions taken.
    float getRandomFraction();

private:
    // Updates the learner if the current frame lends itself to updates. This is
    // done by checking the RAM for when the ALE is accepting actions from the
    // player that will actually have an effect on the game.
    void update(const StateType& state, const ALEScreen& screen);

    // Updates the start and goal colors.
    void updateColors(
        const StateType& state, const ALEScreen& screen, float reward);

    // Assigns the given reward to the learners.
    void update(const StateType& state, float reward);

    // Assigns an additional reward to the learners without updating the state.
    void correctUpdate(float reward);

    // Gets the best action from the learners.
    Action getAction(const StateType& state);

    // Gets Qbert's position from the state.
    std::pair<int, int> getPlayerPosition(const StateType& state);

    // Uses the RAM to find and update Coily's position.
    void fixState(StateType& state);
};
}
