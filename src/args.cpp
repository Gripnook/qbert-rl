#include "args.h"

#include <iostream>

namespace Qbert {

std::pair<std::string, ExplorationPolicy> parseExplorationPolicy(
    const std::string& name, int& argIndex, int argc, char** argv);

Args parseArgs(int argc, char** argv)
{
    Args args;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg{argv[i]};
        if (arg == "-r" || arg == "--rom")
        {
            ++i;
            if (i == argc)
                throw ArgsError{"missing rom file"};
            args.rom = argv[i];
        }
        else if (arg == "-s" || arg == "--seed")
        {
            ++i;
            if (i == argc)
                throw ArgsError{"missing random seed"};
            try
            {
                args.randomSeed = std::stoi(argv[i]);
            }
            catch (...)
            {
                throw ArgsError{"missing random seed"};
            }
        }
        else if (arg == "-x" || arg == "--display_screen")
        {
            args.displayScreen = true;
        }
        else if (arg == "-l" || arg == "--learner")
        {
            ++i;
            if (i == argc)
                throw ArgsError{"missing learner"};
            args.learner = argv[i];
        }
        else if (arg == "-e" || arg == "--exploration_policy")
        {
            ++i;
            if (i == argc)
                throw ArgsError{"missing exploration policy"};
            args.explorationPolicy =
                parseExplorationPolicy(argv[i], i, argc, argv);
        }
        else if (arg == "-h" || arg == "--help")
        {
            args.help = true;
        }
        else if (arg == "-d" || arg == "--debug")
        {
            args.debug = true;
        }
        else
        {
            throw ArgsError{"invalid argument"};
        }
    }

    return args;
}

std::pair<std::string, ExplorationPolicy> parseExplorationPolicy(
    const std::string& name, int& argIndex, int argc, char** argv)
{
    if (name == "inverse_proportional")
    {
        return {"inverse_proportional", ExploreInverseProportional{}};
    }
    else if (name == "epsilon_greedy")
    {
        ++argIndex;
        if (argIndex == argc)
            throw ArgsError{"missing epsilon"};
        try
        {
            float eps = std::stof(argv[argIndex]);
            return {"epsilon_greedy", ExploreEpsilonGreedy{eps}};
        }
        catch (...)
        {
            throw ArgsError{"missing epsilon"};
        }
    }
    else if (name == "threshold")
    {
        ++argIndex;
        if (argIndex == argc)
            throw ArgsError{"missing threshold"};
        try
        {
            int threshold = std::stoi(argv[argIndex]);
            return {"threshold", ExploreThreshold{threshold}};
        }
        catch (...)
        {
            throw ArgsError{"missing threshold"};
        }
    }
    else
    {
        throw ArgsError{"invalid exploration policy"};
    }
}

void printUsage(const char* progname)
{
    Args args;
    std::cerr << "Usage: " << progname << " [options]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "    -r <rom_file>" << std::endl;
    std::cerr << "    --rom <rom_file>" << std::endl;
    std::cerr << "        Sets the ROM file for the game." << std::endl;
    std::cerr << "        Defaults to " << args.rom << "." << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -s <random_seed>" << std::endl;
    std::cerr << "    --seed <random_seed>" << std::endl;
    std::cerr << "        Sets the random seed used by the ALE." << std::endl;
    std::cerr << "        Defaults to " << args.randomSeed << "." << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -x" << std::endl;
    std::cerr << "    --display_screen" << std::endl;
    std::cerr << "        Enables the SDL display for the game." << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -l <learner>" << std::endl;
    std::cerr << "    --learner <learner>" << std::endl;
    std::cerr << "        Sets the learner used for the game." << std::endl;
    std::cerr << "        The possible options are:" << std::endl;
    std::cerr
        << "            monolithic - A monolithic learner that uses a unified"
        << std::endl;
    std::cerr
        << "                state representation for block colors and enemies."
        << std::endl;
    std::cerr << "            subsumption-v1 - A learner with a subsumption"
              << std::endl;
    std::cerr
        << "                architecture that splits the state representation"
        << std::endl;
    std::cerr << "                for block colors and enemies." << std::endl;
    std::cerr
        << "            subsumption-v2 - Same as subsumption-v1, but Coily"
        << std::endl;
    std::cerr << "                is encoded separately from the other enemies."
              << std::endl;
    std::cerr
        << "            subsumption-v3 - Same as subsumption-v2, but Coily"
        << std::endl;
    std::cerr
        << "                is encoded at a greater distance from the player."
        << std::endl;
    std::cerr << "        Defaults to " << args.learner << "." << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -e <exploration_policy>" << std::endl;
    std::cerr << "    --exploration_policy <exploration_policy>" << std::endl;
    std::cerr << "        Sets the exploration policy used by the learner."
              << std::endl;
    std::cerr << "        The possible options are:" << std::endl;
    std::cerr
        << "            inverse_proportional - Selects a random action with"
        << std::endl;
    std::cerr
        << "                probability 1/(N+1) for a state visited N times."
        << std::endl;
    std::cerr
        << "            epsilon_greedy <epsilon> - Selects a random action"
        << std::endl;
    std::cerr << "                with probability epsilon." << std::endl;
    std::cerr
        << "            threshold <threshold> - Selects a random action if a"
        << std::endl;
    std::cerr
        << "                state is visited less times than the threshold."
        << std::endl;
    std::cerr << "        Defaults to " << args.explorationPolicy.first << "."
              << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -h" << std::endl;
    std::cerr << "    --help" << std::endl;
    std::cerr << "        Prints usage information." << std::endl;
    std::cerr << std::endl;
    std::cerr << "    -d" << std::endl;
    std::cerr << "    --debug" << std::endl;
    std::cerr << "        Enables debug information for the game." << std::endl;
}
}
