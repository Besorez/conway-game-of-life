#include "LifeIO.h"

#include <string>
#include <sstream>

#include "LifeLog.h"

namespace LifeIO
{
    static bool IsWhitespaceOnly(const std::string& line)
    {
        for (char ch : line)
        {
            if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')
            {
                return false;
            }
        }
        return true;
    }

    static bool IsCommentLine(const std::string& line)
    {
        for (char ch : line)
        {
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            {
                continue;
            }
            return ch == '#';
        }
        return false;
    }

    static bool ParseCoord(const std::string& line, FCellCoord& outCoord)
    {
        std::istringstream ss(line);

        std::int64_t x = 0;
        std::int64_t y = 0;

        ss >> x >> y;
        if (!ss)
        {
            return false;
        }

        outCoord.X = x;
        outCoord.Y = y;
        return true;
    }

    static void AddLine(FReadStats* stats)
    {
        if (stats != nullptr)
        {
            stats->TotalLines += 1;
        }
    }

    static void AddSkipped(FReadStats* stats)
    {
        if (stats != nullptr)
        {
            stats->SkippedLines += 1;
        }
    }

    static bool ReadHeader(std::istream& in, FReadStats* stats)
    {
        std::string line;

        while (std::getline(in, line))
        {
            AddLine(stats);

            if (IsWhitespaceOnly(line))
            {
                AddSkipped(stats);
                continue;
            }

            if (line == "#Life 1.06")
            {
                return true;
            }

            LIFE_LOG_ERROR("Invalid header. Expected '#Life 1.06'. Got: %s", line.c_str());
            return false;
        }

        LIFE_LOG_ERROR("Missing header '#Life 1.06'.");
        return false;
    }

    bool ReadLife106(std::istream& in, FAliveSet& outAliveSet, FAliveOrdered& outAliveOrdered, FReadStats* outStats)
    {
        outAliveSet.clear();
        outAliveOrdered.clear();

        if (outStats != nullptr)
        {
            *outStats = FReadStats{};
        }

        if (!ReadHeader(in, outStats))
        {
            return false;
        }

        std::string line;

        while (std::getline(in, line))
        {
            AddLine(outStats);

            if (IsWhitespaceOnly(line) || IsCommentLine(line))
            {
                AddSkipped(outStats);
                continue;
            }

            FCellCoord coord;
            if (!ParseCoord(line, coord))
            {
                AddSkipped(outStats);
                LIFE_LOG_WARN("Skipped invalid coord line: %s", line.c_str());
                continue;
            }

            const auto insertResult = outAliveSet.insert(coord);
            if (insertResult.second)
            {
                outAliveOrdered.push_back(coord);
            }
            else
            {
                if (outStats != nullptr)
                {
                    outStats->Duplicates += 1;
                }
            }

            if (outStats != nullptr)
            {
                outStats->ParsedCells += 1;
            }
        }

        return true;
    }

    void WriteLife106(std::ostream& out, const FAliveOrdered& aliveOrdered)
    {
        out << "#Life 1.06\n";

        for (const FCellCoord& c : aliveOrdered)
        {
            out << c.X << " " << c.Y << "\n";
        }
    }
}
