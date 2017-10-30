#include "image-processor.h"

#include <vector>
#include <unordered_map>
#include <tuple>

namespace Qbert {

static constexpr int numBlocks = 21;

static constexpr int width = 320;
static constexpr int height = 210;
static constexpr int xPositions[]{136, 112, 168, 88,  136, 192, 64,
                                  112, 168, 216, 40,  88,  136, 192,
                                  240, 16,  64,  112, 168, 216, 264};
static constexpr int yPositions[]{36,  64,  64,  93,  93,  93,  122,
                                  122, 122, 122, 151, 151, 151, 151,
                                  151, 180, 180, 180, 180, 180, 180};

static_assert(sizeof(xPositions) == sizeof(int) * numBlocks, "");
static_assert(sizeof(yPositions) == sizeof(int) * numBlocks, "");

static constexpr int xBlockSize = 40;
static constexpr int yBlockSize = 5;
static constexpr int xBlockStart = 0;
static constexpr int yBlockStart = -2;

static constexpr int xEntitySize = 8;
static constexpr int yEntitySize = 24;
static constexpr int xEntityStart = 16;
static constexpr int yEntityStart = -26;

static constexpr int numDiscRows = 6;

static constexpr int xDiscPositions[]{
    176, 208, 232, 256, 280, 304, 120, 96, 72, 48, 24, 0};
static constexpr int yDiscPositions[]{
    33, 61, 90, 119, 148, 177, 33, 61, 90, 119, 148, 177};

static_assert(sizeof(xDiscPositions) == sizeof(int) * 2 * numDiscRows, "");
static_assert(sizeof(yDiscPositions) == sizeof(int) * 2 * numDiscRows, "");

static constexpr int xDiscSize = 16;
static constexpr int yDiscSize = 2;
static constexpr int xDiscStart = 0;
static constexpr int yDiscStart = -10;

std::pair<std::vector<GameEntity>, std::vector<Color>>
    extractFeatures(const Screen& screen);

bool isOrange(const Color& color);
bool isPurple(const Color& color);
std::tuple<int, int, int> rgb(const Color& color);

void addDiscs(Grid<GameEntity>& entities, const Screen& screen);

StateType getState(const Screen& screen)
{
    auto p = extractFeatures(screen);
    Grid<GameEntity> entities{};
    Grid<Color> colors{};

    int index = 0;
    for (int i = 1; i < 7; ++i)
    {
        for (int j = 1; j <= i; ++j)
        {
            entities[i - j + 1][j] = p.first[index];
            colors[i - j + 1][j] = p.second[index];
            ++index;
        }
    }

    addDiscs(entities, screen);

    return {entities, colors};
}

std::pair<std::vector<GameEntity>, std::vector<Color>>
    extractFeatures(const Screen& screen)
{
    std::vector<GameEntity> entities;
    std::vector<Color> colors;

    int xScale = width / screen.width();
    int yScale = height / screen.height();

    for (int i = 0; i < numBlocks; ++i)
    {
        int x = xPositions[i] + xEntityStart;
        int y = yPositions[i] + yEntityStart;
        std::unordered_map<Color, int> counts;
        for (int r = y / yScale; r < (y + yEntitySize) / yScale; ++r)
        {
            for (int c = x / xScale; c < (x + xEntitySize) / xScale; ++c)
            {
                Color color = screen.get(r, c);
                if (color != 0)
                {
                    ++counts[color];
                }
            }
        }
        int maxCount = 0;
        Color maxColor = 0;
        for (auto p : counts)
        {
            if (p.second > maxCount)
            {
                maxCount = p.second;
                maxColor = p.first;
            }
        }
        if (counts.empty())
        {
            entities.push_back(GameEntity::None);
        }
        else if (isOrange(maxColor))
        {
            entities.push_back(GameEntity::Qbert);
        }
        else if (isPurple(maxColor))
        {
            entities.push_back(GameEntity::PurpleEnemy);
        }
        else
        {
            entities.push_back(GameEntity::GreenEnemy);
        }
    }

    for (int i = 0; i < numBlocks; ++i)
    {
        int x = xPositions[i] + xBlockStart;
        int y = yPositions[i] + yBlockStart;
        std::unordered_map<Color, int> counts;
        for (int r = y / yScale; r < (y + yBlockSize) / yScale; ++r)
        {
            for (int c = x / xScale; c < (x + xBlockSize) / xScale; ++c)
            {
                Color color = screen.get(r, c);
                if (color != 0)
                {
                    ++counts[color];
                }
            }
        }
        int maxCount = 0;
        Color maxColor = 0;
        for (auto p : counts)
        {
            if (p.second > maxCount)
            {
                maxCount = p.second;
                maxColor = p.first;
            }
        }
        colors.push_back(maxColor);
    }

    return {entities, colors};
}

bool isOrange(const Color& color)
{
    int red, green, blue;
    std::tie(red, green, blue) = rgb(color);
    return red >= 128 && green < 128 && blue < 128;
}

bool isPurple(const Color& color)
{
    int red, green, blue;
    std::tie(red, green, blue) = rgb(color);
    return red >= 128 && green < 128 && blue >= 128;
}

std::tuple<int, int, int> rgb(const Color& color)
{
    int red = (color & 0x00FF0000) >> 16;
    int green = (color & 0x0000FF00) >> 8;
    int blue = (color & 0x000000FF);
    return std::make_tuple(red, green, blue);
}

void addDiscs(Grid<GameEntity>& entities, const Screen& screen)
{
    int xScale = width / screen.width();
    int yScale = height / screen.height();

    for (int i = 0; i < numDiscRows; ++i)
    {
        int x = xDiscPositions[i] + xDiscStart;
        int y = yDiscPositions[i] + yDiscStart;
        std::unordered_map<Color, int> counts;
        for (int r = y / yScale; r < (y + yDiscSize) / yScale; ++r)
        {
            for (int c = x / xScale; c < (x + xDiscSize) / xScale; ++c)
            {
                Color color = screen.get(r, c);
                if (color != 0)
                {
                    ++counts[color];
                }
            }
        }
        if (!counts.empty())
        {
            entities[0][i + 1] = GameEntity::Disc;
        }
    }
    for (int i = numDiscRows; i < 2 * numDiscRows; ++i)
    {
        int x = xDiscPositions[i] + xDiscStart;
        int y = yDiscPositions[i] + yDiscStart;
        std::unordered_map<Color, int> counts;
        for (int r = y / yScale; r < (y + yDiscSize) / yScale; ++r)
        {
            for (int c = x / xScale; c < (x + xDiscSize) / xScale; ++c)
            {
                Color color = screen.get(r, c);
                if (color != 0)
                {
                    ++counts[color];
                }
            }
        }
        if (!counts.empty())
        {
            entities[i + 1 - numDiscRows][0] = GameEntity::Disc;
        }
    }
}
}
