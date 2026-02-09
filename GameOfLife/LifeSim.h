#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <array>

#include "LifeIO.h"

class FLifeSimulator
{
public:
    using FAliveSet = LifeIO::FAliveSet;
    using FAliveOrdered = LifeIO::FAliveOrdered;

    void Step(const FAliveOrdered& currentOrdered, const FAliveSet& currentSet, FAliveOrdered& outNextOrdered, FAliveSet& outNextSet);

private:
    struct FNeighborOffset
    {
        int Dx = 0;
        int Dy = 0;
    };

    static constexpr int kNeighborOffsetCount = 8;
    static const std::array<FNeighborOffset, kNeighborOffsetCount> NeighborOffsets;

    static constexpr std::uint8_t kMinSurviveNeighbors = 2;
    static constexpr std::uint8_t kMaxSurviveNeighbors = 3;
    static constexpr std::uint8_t kBirthNeighbors = 3;

    using FNeighborCounts = std::unordered_map<FCellCoord, std::uint8_t, FCellCoordHash>;
    FNeighborCounts NeighborCounts;

    static bool TryAddDeltaInt64(std::int64_t base, int delta, std::int64_t& outValue);
    static bool TryMakeNeighbor(const FCellCoord& cell, const FNeighborOffset& offset, FCellCoord& outNeighbor);

    void BuildNeighborCounts(const FAliveSet& currentSet);

    static bool ShouldBeAlive(bool bAliveNow, std::uint8_t neighborCount);
};
