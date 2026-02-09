#include <iostream>

#include "LifeIO.h"
#include "LifeSim.h"
#include "LifeLog.h"

int main()
{
    static constexpr int kIterations = 10;

    LifeIO::FAliveSet aliveSet;
    LifeIO::FAliveOrdered aliveOrdered;

    LifeIO::FReadStats stats;
    if (!LifeIO::ReadLife106(std::cin, aliveSet, aliveOrdered, &stats))
    {
        LIFE_LOG_ERROR("Input parse failed.");
        return 1;
    }

    LIFE_LOG_INFO("Input OK. Alive=%zu, Lines=%zu, Skipped=%zu, ParsedCells=%zu, Duplicates=%zu",
        aliveSet.size(), stats.TotalLines, stats.SkippedLines, stats.ParsedCells, stats.Duplicates);

    FLifeSimulator simulator;

    LifeIO::FAliveSet nextSet;
    LifeIO::FAliveOrdered nextOrdered;

    for (int iteration = 0; iteration < kIterations; ++iteration)
    {
        simulator.Step(aliveOrdered, aliveSet, nextOrdered, nextSet);
        aliveOrdered.swap(nextOrdered);
        aliveSet.swap(nextSet);
    }

    LifeIO::WriteLife106(std::cout, aliveOrdered);
    return 0;
}
