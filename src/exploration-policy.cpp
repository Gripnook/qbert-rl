#include "exploration-policy.h"

#include <cstdlib>

namespace Qbert {

ExploreEpsilonGreedy::ExploreEpsilonGreedy(float eps) : eps{eps}
{
}

bool ExploreEpsilonGreedy::operator()(int /*visitCount*/)
{
    float r = static_cast<float>(rand()) / RAND_MAX;
    return r <= eps;
}

bool ExploreInverseProportional::operator()(int visitCount)
{
    return rand() % (visitCount + 1) == 0;
}

ExploreThreshold::ExploreThreshold(int threshold) : threshold{threshold}
{
}

bool ExploreThreshold::operator()(int visitCount)
{
    return visitCount < threshold;
}
}
