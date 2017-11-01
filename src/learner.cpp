#include "learner.h"

#include "game-entity.h"
#include "state.h"

namespace Qbert {

// TODO
static constexpr float alpha = 0.10;
static constexpr float gamma = 0.90;

void Learner::update(
    std::pair<int, int> position,
    const StateType& state,
    float reward,
    Color startColor,
    Color goalColor)
{
    lastState = currentState;
    currentState =
        encode(state, position.first, position.second, startColor, goalColor);

    if (lastState != -1)
    {
        int actionIndex = actionToIndex(currentAction);
        auto q = utilities[lastState][actionIndex];
        auto actions = getActions(position, state);
        auto qMax = utilities[currentState][actionToIndex(*std::max_element(
            actions.begin(), actions.end(), [&](auto lhs, auto rhs) {
                return utilities[currentState][actionToIndex(lhs)] <
                    utilities[currentState][actionToIndex(rhs)];
            }))];
        utilities[lastState][actionIndex] +=
            alpha * (reward + gamma * qMax - q);
    }

    lastAction = currentAction;
    currentAction = tentativeAction;
    ++visited[currentState][actionToIndex(currentAction)];
}

void Learner::correctUpdate(float reward)
{
    if (lastState != -1)
    {
        int actionIndex = actionToIndex(lastAction);
        utilities[lastState][actionIndex] += alpha * reward;
    }
}

Action Learner::getAction(
    std::pair<int, int> position,
    const StateType& state,
    Color startColor,
    Color goalColor)
{
    auto currentState =
        encode(state, position.first, position.second, startColor, goalColor);
    auto actions = getActions(position, state);

    int minVisited = visited[currentState][actionToIndex(*std::min_element(
        actions.begin(), actions.end(), [&](auto lhs, auto rhs) {
            return visited[currentState][actionToIndex(lhs)] <
                visited[currentState][actionToIndex(rhs)];
        }))];

    if (rand() % (minVisited + 1) == 0)
    {
        tentativeAction = actions[rand() % actions.size()];
        return tentativeAction;
    }
    else
    {
        auto qMax = utilities[currentState][actionToIndex(*std::max_element(
            actions.begin(), actions.end(), [&](auto lhs, auto rhs) {
                return utilities[currentState][actionToIndex(lhs)] <
                    utilities[currentState][actionToIndex(rhs)];
            }))];
        std::vector<Action> bestActions;
        for (auto action : actions)
            if (utilities[currentState][actionToIndex(action)] == qMax)
                bestActions.push_back(action);
        tentativeAction = bestActions[rand() % bestActions.size()];
        return tentativeAction;
    }
}

std::vector<Action>
    Learner::getActions(std::pair<int, int> position, const StateType& state)
{
    std::vector<Action> actions;
    if (state.first[position.first - 1][position.second] != GameEntity::Void)
        actions.push_back(Action::PLAYER_A_UP);
    if (state.first[position.first][position.second + 1] != GameEntity::Void)
        actions.push_back(Action::PLAYER_A_RIGHT);
    if (state.first[position.first][position.second - 1] != GameEntity::Void)
        actions.push_back(Action::PLAYER_A_LEFT);
    if (state.first[position.first + 1][position.second] != GameEntity::Void)
        actions.push_back(Action::PLAYER_A_DOWN);
    return actions;
}

int Learner::actionToIndex(const Action& action)
{
    return action - 2;
}

void Learner::reset()
{
    currentState = -1;
    lastState = -1;
    currentAction = Action::PLAYER_A_NOOP;
    lastAction = Action::PLAYER_A_NOOP;
    tentativeAction = Action::PLAYER_A_NOOP;
}
}
