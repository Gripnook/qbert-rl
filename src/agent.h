#pragma once

#include <utility>

#include <ale/ale_interface.hpp>

#include "feature-extractor.h"

namespace Qbert {

// A class that implements a learning agent. This class is responsible for
// dealing with the technicalities of the ALE, such as finding the right frames
// to trigger updates on and getting the state representation. Subclasses should
// implement specific learner structures.
class Agent
{
    ALEInterface& ale;

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

    virtual ~Agent() = default;

    // Updates the state of the game. This should be called every frame.
    void updateState();

    // Resets the agent after a game over.
    virtual void resetGame();

    // Returns the current score for this game.
    float getScore();

    // Returns the best score seen so far.
    float getHighScore();

    // Returns the fraction of random actions taken.
    virtual float getRandomFraction() = 0;

private:
    // Updates the learner if the current frame lends itself to updates. This is
    // done by checking the RAM for when the ALE is accepting actions from the
    // player that will actually have an effect on the game.
    void update(const StateType& state, const ALEScreen& screen);

    // Updates the start and goal colors.
    void updateColors(
        const StateType& state, const ALEScreen& screen, float reward);

    // Assigns the given reward to the learners.
    virtual void update(
        std::pair<int, int> position,
        const StateType& state,
        const Action& actionPerformed,
        float reward,
        Color startColor,
        Color goalColor,
        int level) = 0;

    // Assigns an additional reward to the learners without updating the state.
    virtual void correctUpdate(float reward) = 0;

    // Gets the best action from the learners.
    virtual Action getAction(
        std::pair<int, int> position,
        const StateType& state,
        Color startColor,
        Color goalColor,
        int level) = 0;

    // Gets Qbert's position from the state.
    std::pair<int, int> getPlayerPosition(const StateType& state);
};
}
