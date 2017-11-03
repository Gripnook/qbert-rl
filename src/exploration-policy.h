#pragma once

#include <functional>

namespace Qbert {

// A function that defines if a random action should be chosen for a state whose
// actions have been visited a minimum of visitCount times.
using ExplorationPolicy = std::function<bool(int visitCount)>;

// An exploration policy that selects random actions with probability eps.
class ExploreEpsilonGreedy
{
    const float eps;

public:
    ExploreEpsilonGreedy(float eps);

    bool operator()(int visitCount);
};

// An exploration policy that selects random actions
// with probability 1 / (visitCount + 1).
class ExploreInverseProportional
{
public:
    bool operator()(int visitCount);
};

// An exploration policy that selects random actions
// whenever visitCount < threshold.
class ExploreThreshold
{
    const int threshold;

public:
    ExploreThreshold(int threshold);

    bool operator()(int visitCount);
};
}
