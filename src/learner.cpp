#include "learner.h"

#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "game-entity.h"

namespace Qbert {

Learner::Learner(
    std::string name,
    StateEncoding encodeState,
    ExplorationPolicy explore,
    bool allowNoop,
    float alpha,
    float gamma)
    : name{name},
      encodeState{encodeState},
      explore{explore},
      allowNoop{allowNoop},
      alpha{alpha},
      gamma{gamma}
{
    loadFromFile();
}

void Learner::update(
    std::pair<int, int> position,
    const StateType& state,
    const Action& actionPerformed,
    float reward,
    Color startColor,
    Color goalColor,
    int level)
{
    lastState = currentState;
    currentState = encodeState(
        state, position.first, position.second, startColor, goalColor, level);

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
    currentAction = actionPerformed;
    ++visited[currentState][actionToIndex(currentAction)];
    if (visited[currentState][actionToIndex(currentAction)] == 1000000000)
        --visited[currentState][actionToIndex(currentAction)]; // Avoids
                                                               // overflow.
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
    Color goalColor,
    int level)
{
    auto currentState = encodeState(
        state, position.first, position.second, startColor, goalColor, level);
    auto actions = getActions(position, state);

    int minVisited = visited[currentState][actionToIndex(*std::min_element(
        actions.begin(), actions.end(), [&](auto lhs, auto rhs) {
            return visited[currentState][actionToIndex(lhs)] <
                visited[currentState][actionToIndex(rhs)];
        }))];

    // If we didn't explore the actions in this state enough, we choose a random
    // action to allow the agent more opportunity to learn.
    if (explore(minVisited))
    {
        auto tentativeAction = actions[rand() % actions.size()];
        isRandomAction = true;
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
        auto tentativeAction = bestActions[rand() % bestActions.size()];
        isRandomAction = false;
        return tentativeAction;
    }
}

void Learner::notifyActionTaken()
{
    if (isRandomAction)
        ++randomActionCount;
    ++totalActionCount;
}

std::vector<Action>
    Learner::getActions(std::pair<int, int> position, const StateType& state)
{
    std::vector<Action> actions;
    if (allowNoop)
        actions.push_back(Action::PLAYER_A_NOOP);
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
    return action == Action::PLAYER_A_NOOP ? 0 : action - 1;
}

void Learner::reset()
{
    currentState = -1;
    lastState = -1;
    currentAction = Action::PLAYER_A_NOOP;
    lastAction = Action::PLAYER_A_NOOP;
    randomActionCount = 0;
    totalActionCount = 0;
    isRandomAction = true;

    saveToFile();
}

float Learner::getRandomActionCount()
{
    return randomActionCount;
}

float Learner::getTotalActionCount()
{
    return totalActionCount;
}

float Learner::getRandomFraction()
{
    return totalActionCount == 0 ? 0 : randomActionCount / totalActionCount;
}

void Learner::loadFromFile()
{
    std::ifstream is{"params/" + name + ".param"};
    if (!is)
        return;
    int size;
    is >> size;
    for (int i = 0; i < size; ++i)
    {
        int state;
        is >> state;
        std::array<float, 5> utility;
        for (int j = 0; j < 5; ++j)
            is >> utility[j];
        utilities[state] = utility;
    }
    is >> size;
    for (int i = 0; i < size; ++i)
    {
        int state;
        is >> state;
        std::array<int, 5> counts;
        for (int j = 0; j < 5; ++j)
            is >> counts[j];
        visited[state] = counts;
    }
}

void Learner::saveToFile()
{
    std::ofstream os{"params/" + name + ".param.temp"};
    os << utilities.size() << std::endl;
    for (const auto& p : utilities)
    {
        os << p.first << " ";
        for (const auto& utility : p.second)
            os << utility << " ";
        os << std::endl;
    }
    os << visited.size() << std::endl;
    for (const auto& p : visited)
    {
        os << p.first << " ";
        for (const auto& count : p.second)
            os << count << " ";
        os << std::endl;
    }
    commitFile();
}

void Learner::commitFile()
{
    rename(
        ("params/" + name + ".param.temp").c_str(),
        ("params/" + name + ".param").c_str());
}
}
