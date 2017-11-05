#include "feature-extractor.h"

#include <vector>
#include <unordered_map>
#include <cmath>

namespace Qbert {

// Screen dimensions for which the x and y coordinates were measured.

static constexpr float width = 320;
static constexpr float height = 210;

// Locations of the rectangles that are used to grab the block color and entity
// color information. Each position pair corresponds to an absolute position in
// the image, and the start variables are relative to that fixed point.

static constexpr int numBlocks = 21;

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

// Locations of the rectangles that are used to grab the disc locations. Each
// position pair corresponds to an absolute position in the image, and the start
// variables are relative to that fixed point.

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

// Location of the rectangle that is used to grab the goal color information
// from the score display. The start position corresponds to an absolute
// position in the image.

static constexpr int xGoalSize = 10;
static constexpr int yGoalSize = 7;
static constexpr int xGoalStart = 68;
static constexpr int yGoalStart = 6;

// Location of the rectangle that is used to grab the background color in the
// image. The start position corresponds to an absolute position in the image.

static constexpr int xBackgroundSize = 8;
static constexpr int yBackgroundSize = 16;
static constexpr int xBackgroundStart = 0;
static constexpr int yBackgroundStart = 0;

// Processes the image on the screen to obtain the locations of Qbert and the
// other entities, as well as the colors of all the blocks in the game. Note
// that this method does not distinguish between enemies with the same color,
// such as Coily and the purple ball.
StateType getState(const ALEScreen& screen);

// Extracts the game entities and block colors from the screen and returns them
// in vectors indexed in a way that traverses the pyramid from the top-down and
// from left to right.
std::pair<std::vector<GameEntity>, std::vector<Color>>
    extractFeatures(const ALEScreen& screen);

// Is this Qbert's color?
bool isQbert(const Color& color);

// Is this a purple enemy's color?
bool isPurpleEnemy(const Color& color);

// Is this a green enemy's color?
bool isGreenEnemy(const Color& color);

// Replaces the generic purple enemy with Coily.
void addCoily(StateType& state, const ALERAM& ram);

// Replaces the voids that have discs with discs.
void addDiscs(Grid<GameEntity>& entities, const ALEScreen& screen);

// Extracts the background color from the screen.
Color getBackground(const ALEScreen& screen);

// Extracts a map of colors to number of pixels with those colors within a given
// rectangle. This function also filters out the background color given, as well
// as the color black (0x00).
std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen,
    int rowBegin,
    int rowEnd,
    int colBegin,
    int colEnd,
    Color background);

// Extracts a map of colors to number of pixels with those colors within a given
// rectangle.
std::unordered_map<Color, int> getColorCounts(
    const ALEScreen& screen,
    int rowBegin,
    int rowEnd,
    int colBegin,
    int colEnd);

// Returns the color that has the greatest count in the given map.
Color getMaxColor(const std::unordered_map<Color, int>& counts);

// Rounds the argument to the nearest integer.
int round(float arg);

StateType getState(ALEInterface& ale)
{
    auto state = getState(ale.getScreen());
    addCoily(state, ale.getRAM());
    return state;
}

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

    auto xScale = width / screen.width();
    auto yScale = height / screen.height();

    Color background = getBackground(screen);

    // Extracts the game entities.
    for (int i = 0; i < numBlocks; ++i)
    {
        int x0 = xPositions[i] + xEntityStart;
        int x1 = x0 + xEntitySize;
        int y0 = yPositions[i] + yEntityStart;
        int y1 = y0 + yEntitySize;
        auto counts = getColorCounts(
            screen,
            round(y0 / yScale),
            round(y1 / yScale),
            round(x0 / xScale),
            round(x1 / xScale),
            background);
        Color maxColor = getMaxColor(counts);
        if (counts.empty())
            entities.push_back(GameEntity::None);
        else if (isQbert(maxColor))
            entities.push_back(GameEntity::Qbert);
        else if (isPurpleEnemy(maxColor))
            entities.push_back(GameEntity::PurpleBall); // We add a purple ball
                                                        // for now, and we will
                                                        // correct with Coily's
                                                        // position in RAM
                                                        // later.
        else if (
            isGreenEnemy(maxColor) && counts[maxColor] > 80 / (xScale * yScale))
            entities.push_back(GameEntity::Sam);
        else if (isGreenEnemy(maxColor))
            entities.push_back(GameEntity::GreenBall);
        else
            entities.push_back(GameEntity::RedBall);
    }

    // Extracts the block colors.
    for (int i = 0; i < numBlocks; ++i)
    {
        int x0 = xPositions[i] + xBlockStart;
        int x1 = x0 + xBlockSize;
        int y0 = yPositions[i] + yBlockStart;
        int y1 = y0 + yBlockSize;
        auto counts = getColorCounts(
            screen,
            round(y0 / yScale),
            round(y1 / yScale),
            round(x0 / xScale),
            round(x1 / xScale),
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

void addCoily(StateType& state, const ALERAM& ram)
{
    int xCoily = ram.get(0x27);
    int yCoily = ram.get(0x45);
    // Converts the diagonal position given in RAM to our coordinate system.
    int x = (xCoily - yCoily + 5) / 2 + 1;
    int y = (xCoily + yCoily - 5) / 2 + 1;
    if (x >= 0 && x < 8 && y >= 0 && y < 8 && state.second[x][y] != 0)
        state.first[x][y] = GameEntity::Coily;
}

void addDiscs(Grid<GameEntity>& entities, const ALEScreen& screen)
{
    auto xScale = width / screen.width();
    auto yScale = height / screen.height();

    for (int i = 0; i < numDiscs; ++i)
    {
        int x0 = xDiscPositions[i] + xDiscStart;
        int x1 = x0 + xDiscSize;
        int y0 = yDiscPositions[i] + yDiscStart;
        int y1 = y0 + yDiscSize;
        auto counts = getColorCounts(
            screen,
            round(y0 / yScale),
            round(y1 / yScale),
            round(x0 / xScale),
            round(x1 / xScale));

        // Discs have uniform, non-black coloring.
        if (counts.size() == 1 && counts.count(0) == 0)
            entities[discRows[i]][discCols[i]] = GameEntity::Disc;
    }
}

Color getGoalColor(const ALEScreen& screen)
{
    auto xScale = width / screen.width();
    auto yScale = height / screen.height();

    Color background = getBackground(screen);

    auto counts = getColorCounts(
        screen,
        round(yGoalStart / yScale),
        round((yGoalStart + yGoalSize) / yScale),
        round(xGoalStart / xScale),
        round((xGoalStart + xGoalSize) / xScale),
        background);

    // If it's all background, then the goal color isn't displayed.
    return counts.empty() ? 0 : getMaxColor(counts);
}

Color getBackground(const ALEScreen& screen)
{
    auto xScale = width / screen.width();
    auto yScale = height / screen.height();

    auto counts = getColorCounts(
        screen,
        round(yBackgroundStart / yScale),
        round((yBackgroundStart + yBackgroundSize) / yScale),
        round(xBackgroundStart / xScale),
        round((xBackgroundStart + xBackgroundSize) / xScale),
        0);

    // If it's all black, then the background is black.
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
    auto counts = getColorCounts(screen, rowBegin, rowEnd, colBegin, colEnd);
    counts.erase(0);
    counts.erase(background);
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

int round(float arg)
{
    return static_cast<int>(std::round(arg));
}
}
