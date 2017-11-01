#include <iostream>
#include <fstream>
#include <iomanip>

#include <ale/ale_interface.hpp>

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#include "image-processor.h"
#include "game-entity.h"
#include "agent.h"

using namespace Qbert;

void print(const StateType& state);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
        return 1;
    }

    ALEInterface ale;
    ale.setInt("random_seed", 123);
#ifdef USE_SDL
    ale.setBool("display_screen", true);
    ale.setBool("sound", true);
#endif
    ale.loadROM(argv[1]);

    Agent agent{ale};

    std::ofstream os{"results/scores.csv"};
    os << "Episode,Score" << std::endl;
    int episode = 0;
    while (true)
    {
        ++episode;
        while (!ale.game_over())
        {
            // auto state = getState(ale.getScreen());
            // std::cout << "High Score: " << agent.getHighScore() << std::endl;
            // print(state);
            agent.update();
        }
        os << episode << "," << agent.getScore() << std::endl;
        ale.reset_game();
        agent.resetGame();
    }

    return 0;
}

void print(const StateType& state)
{
    std::cout << "ENTITIES" << std::endl;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
            std::cout << toString(state.first[i][j]) << " ";
        std::cout << std::endl;
    }

    std::cout << std::endl << "BLOCK COLORS" << std::endl;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
            std::cout << std::setw(3) << state.second[i][j] << " ";
        std::cout << std::endl;
    }
}
