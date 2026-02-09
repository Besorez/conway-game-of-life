#include "LifeSim.h"

#include <limits>
#include <algorithm>

const std::array<FLifeSimulator::FNeighborOffset, FLifeSimulator::kNeighborOffsetCount> FLifeSimulator::NeighborOffsets =
{
    FLifeSimulator::FNeighborOffset{ -1, -1 },
    FLifeSimulator::FNeighborOffset{  0, -1 },
    FLifeSimulator::FNeighborOffset{  1, -1 },
    FLifeSimulator::FNeighborOffset{ -1,  0 },
    FLifeSimulator::FNeighborOffset{  1,  0 },
    FLifeSimulator::FNeighborOffset{ -1,  1 },
    FLifeSimulator::FNeighborOffset{  0,  1 },
    FLifeSimulator::FNeighborOffset{  1,  1 }
};

bool FLifeSimulator::TryAddDeltaInt64(std::int64_t base, int delta, std::int64_t& outValue)
{
    if (delta == 0)
    {
        outValue = base;
        return true;
    }

    if (delta > 0)
    {
        if (base == (std::numeric_limits<std::int64_t>::max)())
        {
            return false;
        }
        outValue = base + 1;
        return true;
    }

    if (base == (std::numeric_limits<std::int64_t>::min)())
    {
        return false;
    }

    outValue = base - 1;
    return true;
}

bool FLifeSimulator::TryMakeNeighbor(const FCellCoord& cell, const FNeighborOffset& offset, FCellCoord& outNeighbor)
{
    std::int64_t nx = 0;
    std::int64_t ny = 0;

    if (!TryAddDeltaInt64(cell.X, offset.Dx, nx))
    {
        return false;
    }

    if (!TryAddDeltaInt64(cell.Y, offset.Dy, ny))
    {
        return false;
    }

    outNeighbor.X = nx;
    outNeighbor.Y = ny;
    return true;
}

bool FLifeSimulator::ShouldBeAlive(bool bAliveNow, std::uint8_t neighborCount)
{
    if (bAliveNow)
    {
        return neighborCount >= kMinSurviveNeighbors && neighborCount <= kMaxSurviveNeighbors;
    }

    return neighborCount == kBirthNeighbors;
}

void FLifeSimulator::BuildNeighborCounts(const FAliveSet& currentSet)
{
    NeighborCounts.clear();
    NeighborCounts.reserve(currentSet.size() * static_cast<std::size_t>(kNeighborOffsetCount));

    for (const FCellCoord& cell : currentSet)
    {
        for (const FNeighborOffset& offset : NeighborOffsets)
        {
            FCellCoord neighbor;
            if (!TryMakeNeighbor(cell, offset, neighbor))
            {
                continue;
            }

            auto it = NeighborCounts.find(neighbor);
            if (it == NeighborCounts.end())
            {
                NeighborCounts.emplace(neighbor, static_cast<std::uint8_t>(1));
                continue;
            }

            it->second = static_cast<std::uint8_t>(it->second + 1);
        }
    }
}

void FLifeSimulator::Step(const FAliveOrdered& currentOrdered, const FAliveSet& currentSet, FAliveOrdered& outNextOrdered, FAliveSet& outNextSet)
{
    BuildNeighborCounts(currentSet);

    outNextOrdered.clear();
    outNextSet.clear();

    outNextOrdered.reserve(currentOrdered.size());
    outNextSet.reserve(currentSet.size());

    // 1) Survivors in the same order as input/current
    for (const FCellCoord& cell : currentOrdered)
    {
        auto it = NeighborCounts.find(cell);
        const std::uint8_t count = (it != NeighborCounts.end()) ? it->second : static_cast<std::uint8_t>(0);

        const bool bAliveNow = true;
        if (ShouldBeAlive(bAliveNow, count))
        {
            outNextSet.insert(cell);
            outNextOrdered.push_back(cell);
        }
    }

    // 2) Births appended at the end, in stable deterministic order
    // We collect births first, then sort by Y then X purely for stable diffing.
    std::vector<FCellCoord> births;
    births.reserve(NeighborCounts.size());

    for (const auto& pair : NeighborCounts)
    {
        const FCellCoord& cell = pair.first;
        const std::uint8_t count = pair.second;

        const bool bAliveNow = currentSet.find(cell) != currentSet.end();
        if (bAliveNow)
        {
            continue;
        }

        if (ShouldBeAlive(false, count))
        {
            births.push_back(cell);
        }
    }

    auto sortByYThenX = [](const FCellCoord& a, const FCellCoord& b)
        {
            if (a.Y < b.Y) return true;
            if (a.Y > b.Y) return false;
            return a.X < b.X;
        };

    std::sort(births.begin(), births.end(), sortByYThenX);

    for (const FCellCoord& cell : births)
    {
        outNextSet.insert(cell);
        outNextOrdered.push_back(cell);
    }
}
