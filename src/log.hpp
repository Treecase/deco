#pragma once

#include <format>
#include <hyprland/src/debug/Log.hpp>
#include <source_location>
#include <stdexcept>

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

template<class... Args>
[[noreturn]]
inline static void panic(std::format_string<Args...> const& fmt, Args... args)
{
    throw std::runtime_error{std::format(
        "[deco] {}",
        std::vformat(fmt.get(), std::make_format_args(args...)))};
}

template<class Ptr, class... Args>
inline static void
panic_if_null(Ptr ptr, std::format_string<Args...> const& fmt, Args... args)
{
    if (ptr == nullptr) {
        panic(fmt, args...);
    }
}

} // namespace deco
