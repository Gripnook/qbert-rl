#pragma once

#include <vector>

namespace Qbert {

using Color = int;

class Screen
{
    int w, h;
    std::vector<Color> colors;

public:
    Screen(const std::vector<unsigned char>& rgb, int width, int height);
    int width() const;
    int height() const;
    Color get(int row, int col) const;
};
}
