#include "screen.h"

#include <stdexcept>

namespace Qbert {

Screen::Screen(const std::vector<unsigned char>& rgb, int width, int height)
    : w{width}, h{height}
{
    if (static_cast<int>(rgb.size()) != 3 * w * h)
    {
        throw std::runtime_error{"invalid screen RGB size"};
    }

    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            int index = i * w + j;
            Color color = 0;
            color |= rgb[3 * index] << 16;
            color |= rgb[3 * index + 1] << 8;
            color |= rgb[3 * index + 2];
            colors.push_back(color);
        }
    }
}

int Screen::width() const
{
    return w;
}

int Screen::height() const
{
    return h;
}

Color Screen::get(int row, int col) const
{
    return colors[row * w + col];
}
}
