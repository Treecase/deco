#pragma once

#include <hyprland/src/debug/Log.hpp>
#include <source_location>

namespace deco {

template<class... Args>
inline static void
log(eLogLevel level, std::format_string<Args...> const& str, Args... args)
{
    Debug::log(
        level,
        "[deco] {}",
        std::vformat(str.get(), std::make_format_args(args...)));
}

template<class... Args>
inline static void log(std::format_string<Args...> const& str, Args... args)
{
    log(eLogLevel::LOG, str, args...);
}

template<class... Args>
inline static void trace(std::format_string<Args...> const& str, Args... args)
{
    log(eLogLevel::TRACE, str, args...);
}

template<class... Args>
inline static void info(std::format_string<Args...> const& str, Args... args)
{
    log(eLogLevel::INFO, str, args...);
}

template<class... Args>
inline static void err(std::format_string<Args...> const& str, Args... args)
{
    log(eLogLevel::ERR, str, args...);
}

#define TRACE                                              \
    {                                                      \
        auto const here = std::source_location::current(); \
        ::deco::trace("{}", here.function_name());         \
    }

} // namespace deco
