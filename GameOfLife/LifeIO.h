#pragma once

#include <unordered_set>
#include <vector>
#include <iosfwd>
#include <cstddef>

#include "CellCoord.h"

namespace LifeIO
{
    using FAliveSet = std::unordered_set<FCellCoord, FCellCoordHash>;
    using FAliveOrdered = std::vector<FCellCoord>;

    struct FReadStats
    {
        std::size_t TotalLines = 0;
        std::size_t SkippedLines = 0;
        std::size_t ParsedCells = 0;
        std::size_t Duplicates = 0;
    };

    bool ReadLife106(std::istream& in, FAliveSet& outAliveSet, FAliveOrdered& outAliveOrdered, FReadStats* outStats = nullptr);

    void WriteLife106(std::ostream& out, const FAliveOrdered& aliveOrdered);
}
