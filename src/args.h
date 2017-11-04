#pragma once

#include <set>
#include <string>
#include <utility>
#include <stdexcept>

#include "exploration-policy.h"

namespace Qbert {

const std::set<std::string> learners = {"monolithic", "subsumption"};

struct Args
{
    std::string rom{"qbert.bin"};
    int randomSeed{123};
    bool displayScreen{false};

    std::string learner{"subsumption"};
    std::pair<std::string, ExplorationPolicy> explorationPolicy{
        "inverse_proportional", ExploreInverseProportional{}};

    bool help{false};
    bool debug{false};
};

class ArgsError : public std::exception
{
    const std::string message;

public:
    ArgsError(std::string message) : message{message}
    {
    }

    virtual ~ArgsError() = default;

    virtual const char* what() const noexcept override
    {
        return message.c_str();
    }
};

// Parses the command line arguments.
Args parseArgs(int argc, char** argv);

// Prints usage information to std::cerr.
void printUsage(const char* progname);
}
