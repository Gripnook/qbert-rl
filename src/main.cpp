#include <iostream>
#include <iomanip>

#include <ale/ale_interface.hpp>
#include <SDL/SDL.h>

#include "image-processor.h"
#include "screen.h"
#include "game-entity.h"

using namespace Qbert;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
        return 1;
    }

    ALEInterface ale;
    ale.setInt("random_seed", 123);
    ale.setBool("display_screen", true);
    ale.setBool("sound", true);
    ale.loadROM(argv[1]);

    std::vector<unsigned char> rgb;
    while (true)
    {
        std::pair<int, int> playerState{1, 1};
        while (!ale.game_over())
        {
            ale.getScreenRGB(rgb);
            auto state =
                getState(Screen{rgb,
                                static_cast<int>(ale.getScreen().width()),
                                static_cast<int>(ale.getScreen().height())});

            std::cout << "ENTITIES" << std::endl;
            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    std::cout << toString(state.first[i][j]) << " ";
                }
                std::cout << std::endl;
            }

            std::cout << std::endl << "BLOCK COLORS" << std::endl;
            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    std::cout << "0x" << std::setfill('0') << std::hex
                              << std::setw(6) << state.second[i][j] << std::dec
                              << " ";
                }
                std::cout << std::endl;
            }

            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    if (state.first[i][j] == GameEntity::Qbert)
                    {
                        playerState = std::make_pair(i, j);
                    }
                }
            }

            std::vector<Action> actions;
            if (!isDangerous(
                    state.first[playerState.first - 1][playerState.second]))
            {
                actions.push_back(Action::PLAYER_A_UP);
            }
            if (!isDangerous(
                    state.first[playerState.first][playerState.second + 1]))
            {
                actions.push_back(Action::PLAYER_A_RIGHT);
            }
            if (!isDangerous(
                    state.first[playerState.first][playerState.second - 1]))
            {
                actions.push_back(Action::PLAYER_A_LEFT);
            }
            if (!isDangerous(
                    state.first[playerState.first + 1][playerState.second]))
            {
                actions.push_back(Action::PLAYER_A_DOWN);
            }

            if (actions.empty())
            {
                ale.act(Action::PLAYER_A_NOOP);
            }
            else
            {
                Action action = actions[rand() % actions.size()];
                ale.act(action);
            }
        }
        ale.reset_game();
    }

    return 0;
}
