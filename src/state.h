#pragma once

#include "image-processor.h"

namespace Qbert {

int encode(
    const StateType& state, int x, int y, Color startColor, Color goalColor);
}
