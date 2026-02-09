#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>

struct FCellCoord
{
    std::int64_t X = 0;
    std::int64_t Y = 0;

    bool operator==(const FCellCoord& other) const
    {
        return X == other.X && Y == other.Y;
    }
};

struct FCellCoordHash
{
    std::size_t operator()(const FCellCoord& c) const noexcept
    {
        const std::size_t hashX = Hash_Int64(c.X);
        const std::size_t hashY = Hash_Int64(c.Y);

        return Mix_Hashes(hashX, hashY);
    }

private:
    static std::size_t Hash_Int64(std::int64_t value) noexcept
    {
        return std::hash<std::int64_t>{}(value);
    }

    static std::size_t Mix_Hashes(std::size_t a, std::size_t b) noexcept
    {
        /*
            We need a stable hash for a 2D coordinate.

            Goals:
            - Fast
            - Low collision rate for typical spatial distributions
            - Deterministic

            Strategy:
            - Take standard hashes of X and Y
            - Mix them using a simple avalanche step (xor + shifts)

            This is not cryptographic and does not need to be.
        */
        std::size_t mixed = a;

        mixed ^= b;
        mixed ^= (mixed << 7);
        mixed ^= (mixed >> 3);

        return mixed;
    }
};
