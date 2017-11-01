#include "image-processor.h"

#include <vector>
#include <unordered_map>

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

static constexpr int numDiscs = 5;

static constexpr int xDiscPositions[]{176, 232, 280, 72, 24};
static constexpr int yDiscPositions[]{33, 90, 148, 90, 148};
static constexpr int discRows[]{0, 0, 0, 3, 5};
static constexpr int discCols[]{1, 3, 5, 0, 0};

static_assert(sizeof(xDiscPositions) == sizeof(int) * numDiscs, "");
static_assert(sizeof(yDiscPositions) == sizeof(int) * numDiscs, "");
static_assert(sizeof(discRows) == sizeof(int) * numDiscs, "");
static_assert(sizeof(discCols) == sizeof(int) * numDiscs, "");

static constexpr int xDiscSize = 16;
static constexpr int yDiscSize = 2;
static constexpr int xDiscStart = 0;
static constexpr int yDiscStart = -10;

static constexpr int xGoalSize = 10;
static constexpr int yGoalSize = 7;
static constexpr int xGoalStart = 68;
static constexpr int yGoalStart = 6;

static constexpr int xBackgroundSize = 8;
static constexpr int yBackgroundSize = 16;
static constexpr int xBackgroundStart = 0;
static constexpr int yBackgroundStart = 0;

std::pair<std::vector<GameEntity>, std::vector<Color>>
    extractFeatures(const ALEScreen& screen);

bool isQbert(const Color& color);
bool isPurpleEnemy(const Color& color);
bool isGreenEnemy(const Color& color);

void addDiscs(Grid<GameEntity>& entities, const ALEScreen& screen);

Color getBackground(const ALEScreen& screen);
std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen,
    int rowBegin,
    int rowEnd,
    int colBegin,
    int colEnd,
    Color background);
std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen,
    int rowBegin,
    int rowEnd,
    int colBegin,
    int colEnd);
Color getMaxColor(const std::unordered_map<Color, int>& counts);

StateType getState(const ALEScreen& screen)
{
    auto features = extractFeatures(screen);
    Grid<GameEntity> entities{};
    Grid<Color> colors{};

    int index = 0;
    for (int i = 1; i < 7; ++i)
    {
        for (int j = 1; j <= i; ++j)
        {
            entities[i - j + 1][j] = features.first[index];
            colors[i - j + 1][j] = features.second[index];
            ++index;
        }
    }

    addDiscs(entities, screen);

    return {entities, colors};
}

std::pair<std::vector<GameEntity>, std::vector<Color>>
    extractFeatures(const ALEScreen& screen)
{
    std::vector<GameEntity> entities;
    std::vector<Color> colors;

    int xScale = width / screen.width();
    int yScale = height / screen.height();

    Color background = getBackground(screen);

    for (int i = 0; i < numBlocks; ++i)
    {
        int x0 = xPositions[i] + xEntityStart;
        int x1 = x0 + xEntitySize;
        int y0 = yPositions[i] + yEntityStart;
        int y1 = y0 + yEntitySize;
        auto counts = getColorCounts(
            screen,
            y0 / yScale,
            y1 / yScale,
            x0 / xScale,
            x1 / xScale,
            background);
        Color maxColor = getMaxColor(counts);
        if (counts.empty())
            entities.push_back(GameEntity::None);
        else if (isQbert(maxColor))
            entities.push_back(GameEntity::Qbert);
        else if (isPurpleEnemy(maxColor))
            entities.push_back(GameEntity::PurpleEnemy);
        else if (isGreenEnemy(maxColor))
            entities.push_back(GameEntity::GreenEnemy);
        else
            entities.push_back(GameEntity::RedEnemy);
    }

    for (int i = 0; i < numBlocks; ++i)
    {
        int x0 = xPositions[i] + xBlockStart;
        int x1 = x0 + xBlockSize;
        int y0 = yPositions[i] + yBlockStart;
        int y1 = y0 + yBlockSize;
        auto counts = getColorCounts(
            screen,
            y0 / yScale,
            y1 / yScale,
            x0 / xScale,
            x1 / xScale,
            background);
        Color maxColor = getMaxColor(counts);
        colors.push_back(maxColor);
    }

    return {entities, colors};
}

bool isQbert(const Color& color)
{
    return color == 52;
}

bool isPurpleEnemy(const Color& color)
{
    return color == 102;
}

bool isGreenEnemy(const Color& color)
{
    return color == 196;
}

void addDiscs(Grid<GameEntity>& entities, const ALEScreen& screen)
{
    int xScale = width / screen.width();
    int yScale = height / screen.height();

    for (int i = 0; i < numDiscs; ++i)
    {
        int x0 = xDiscPositions[i] + xDiscStart;
        int x1 = x0 + xDiscSize;
        int y0 = yDiscPositions[i] + yDiscStart;
        int y1 = y0 + yDiscSize;
        auto counts = getColorCounts(
            screen, y0 / yScale, y1 / yScale, x0 / xScale, x1 / xScale);

        // Discs have uniform, non-black coloring.
        if (counts.size() == 1 && counts.count(0) == 0)
            entities[discRows[i]][discCols[i]] = GameEntity::Disc;
    }
}

Color getGoalColor(const ALEScreen& screen)
{
    int xScale = width / screen.width();
    int yScale = height / screen.height();

    Color background = getBackground(screen);

    auto counts = getColorCounts(
        screen,
        yGoalStart / yScale,
        (yGoalStart + yGoalSize) / yScale,
        xGoalStart / xScale,
        (xGoalStart + xGoalSize) / xScale,
        background);

    return counts.empty() ? 0 : getMaxColor(counts);
}

Color getBackground(const ALEScreen& screen)
{
    int xScale = width / screen.width();
    int yScale = height / screen.height();

    auto counts = getColorCounts(
        screen,
        yBackgroundStart / yScale,
        (yBackgroundStart + yBackgroundSize) / yScale,
        xBackgroundStart / xScale,
        (xBackgroundStart + xBackgroundSize) / xScale,
        0);

    return counts.empty() ? 0 : getMaxColor(counts);
}

std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen,
    int rowBegin,
    int rowEnd,
    int colBegin,
    int colEnd,
    Color background)
{
    std::unordered_map<Color, int> counts;
    for (int r = rowBegin; r < rowEnd; ++r)
    {
        for (int c = colBegin; c < colEnd; ++c)
        {
            Color color = screen.get(r, c);
            if (color != background && color != 0)
                ++counts[color];
        }
    }
    return counts;
}

std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen, int rowBegin, int rowEnd, int colBegin, int colEnd)
{
    std::unordered_map<Color, int> counts;
    for (int r = rowBegin; r < rowEnd; ++r)
    {
        for (int c = colBegin; c < colEnd; ++c)
        {
            Color color = screen.get(r, c);
            ++counts[color];
        }
    }
    return counts;
}

Color getMaxColor(const std::unordered_map<Color, int>& counts)
{
    int maxCount = 0;
    Color maxColor = 0;
    for (auto count : counts)
    {
        if (count.second > maxCount)
        {
            maxCount = count.second;
            maxColor = count.first;
        }
    }
    return maxColor;
}
}
