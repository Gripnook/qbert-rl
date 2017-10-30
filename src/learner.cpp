#include "learner.h"

#include "screen.h"
#include "game-entity.h"
#include "state.h"

namespace Qbert {

// TODO
static constexpr float alpha = 0.10;
static constexpr float gamma = 0.95;

Action Learner::update(
    std::pair<int, int> position,
    const StateType& state,
    float reward,
    Color goal)
{
    auto lastState = currentState;
    currentState = encode(state, position.first, position.second, goal);
    if (utilities.count(currentState) == 0)
    {
        utilities[currentState] = std::array<float, 4>{};
        visited[currentState] = std::array<int, 4>{};
    }

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

    currentAction = getAction(position, state);
    return currentAction;
}

Action Learner::getAction(std::pair<int, int> position, const StateType& state)
{
    auto actions = getActions(position, state);

    int minVisited = visited[currentState][actionToIndex(*std::min_element(
        actions.begin(), actions.end(), [&](auto lhs, auto rhs) {
            return visited[currentState][actionToIndex(lhs)] <
                visited[currentState][actionToIndex(rhs)];
        }))];

    if (rand() % (minVisited + 1) == 0)
    {
        auto action = actions[rand() % actions.size()];
        ++visited[currentState][actionToIndex(action)];
        return action;
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
        auto action = bestActions[rand() % bestActions.size()];
        ++visited[currentState][actionToIndex(action)];
        return action;
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
}
}
