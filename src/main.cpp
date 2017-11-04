#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>

#include <ale/ale_interface.hpp>

#include "args.h"
#include "image-processor.h"
#include "game-entity.h"
#include "monolithic-agent.h"
#include "subsumption-agent-2.h"
#include "state-encoding.h"

using namespace Qbert;

void learn(const Args& args);
std::unique_ptr<Agent> createAgent(ALEInterface& ale, const Args& args);
void print(const StateType& state);

int main(int argc, char** argv)
{
    try
    {
        auto args = parseArgs(argc, argv);
        if (args.help)
            printUsage(argv[0]);
        else
            learn(args);
        return 0;
    }
    catch (ArgsError& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        return 3;
    }
}

void learn(const Args& args)
{
    ALEInterface ale;
    ale.setInt("random_seed", args.randomSeed);
    ale.setBool("display_screen", args.displayScreen);
    ale.setBool("sound", args.displayScreen);
    ale.setFloat("repeat_action_probability", 0.0f);
    ale.loadROM(args.rom);

    auto agent = createAgent(ale, args);

    std::ofstream os{"results/scores." + args.learner + "." +
                     args.explorationPolicy.first + ".csv"};
    os << "Episode,Score,Random" << std::endl;
    int episode = 0;
    while (true)
    {
        ++episode;
        while (!ale.game_over())
        {
            if (args.debug && ale.getEpisodeFrameNumber() % 20 == 0)
            {
                auto state = getState(ale.getScreen());
                std::cout << "High Score: " << agent->getHighScore()
                          << std::endl;
                print(state);
            }
            agent->updateState();
        }
        os << episode << "," << agent->getScore() << ","
           << agent->getRandomFraction() << std::endl;
        ale.reset_game();
        agent->resetGame();
    }
}

std::unique_ptr<Agent> createAgent(ALEInterface& ale, const Args& args)
{
    if (args.learner == "monolithic")
        return std::make_unique<MonolithicAgent>(
            ale,
            args.learner + "-" + args.explorationPolicy.first,
            encodeState,
            args.explorationPolicy.second);
    else if (args.learner == "subsumption-v1")
        return std::make_unique<SubsumptionAgent2>(
            ale,
            args.learner + "-" + args.explorationPolicy.first,
            encodeBlockState,
            encodeEnemyState,
            hasEnemiesNearby,
            args.explorationPolicy.second);
    else if (args.learner == "subsumption-v2")
        return std::make_unique<SubsumptionAgent2>(
            ale,
            args.learner + "-" + args.explorationPolicy.first,
            encodeBlockState,
            encodeEnemyStateWithSeparateCoily,
            hasEnemiesNearbyWithSeparateCoily,
            args.explorationPolicy.second);
    else
        throw ArgsError{"invalid learner"};
}

void print(const StateType& state)
{
    std::cout << "Game Entities" << std::endl;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
            std::cout << toString(state.first[i][j]) << " ";
        std::cout << std::endl;
    }

    std::cout << std::endl << "Block Colors" << std::endl;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
            std::cout << std::setw(3) << state.second[i][j] << " ";
        std::cout << std::endl;
    }
}
