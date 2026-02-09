#pragma once

#include <cstdarg>
#include <cstdio>

namespace LifeLog
{
    enum class ELevel
    {
        Info,
        Warning,
        Error
    };

    inline const char* ToString(ELevel level)
    {
        switch (level)
        {
        case ELevel::Info: return "INFO";
        case ELevel::Warning: return "WARN";
        case ELevel::Error: return "ERROR";
        default: return "LOG";
        }
    }

    inline void Print(ELevel level, const char* format, ...)
    {
        std::fprintf(stderr, "[%s] ", ToString(level));

        va_list args;
        va_start(args, format);
        std::vfprintf(stderr, format, args);
        va_end(args);

        std::fprintf(stderr, "\n");
        std::fflush(stderr);
    }
}

#define LIFE_LOG_INFO(fmt, ...)  LifeLog::Print(LifeLog::ELevel::Info, fmt, ##__VA_ARGS__)
#define LIFE_LOG_WARN(fmt, ...)  LifeLog::Print(LifeLog::ELevel::Warning, fmt, ##__VA_ARGS__)
#define LIFE_LOG_ERROR(fmt, ...) LifeLog::Print(LifeLog::ELevel::Error, fmt, ##__VA_ARGS__)
