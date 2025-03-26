#pragma once

#include <format>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>

#include "defines.hpp"

namespace deco {

inline HANDLE g_handle{nullptr};

template<class... Args>
inline void log(std::format_string<Args...> const& str, Args... args)
{
    Debug::log(
        LOG,
        "[deco] {}",
        std::vformat(str.get(), std::make_format_args(args...)));
}

template<class... Args>
inline void
log(eLogLevel level, std::format_string<Args...> const& str, Args... args)
{
    Debug::log(
        level,
        "[deco] {}",
        std::vformat(str.get(), std::make_format_args(args...)));
}

inline void notify(
    std::string const& text,
    eIcons icon,
    uint64_t time = 5000,
    CHyprColor const& color = CHyprColor{0})
{
    std::string const msg =
        std::format("[deco {}] {}", DECO_PLUGIN_VERSION, text);
    HyprlandAPI::addNotificationV2(
        deco::g_handle,
        {
            { "text",  std::make_any<std::string>(msg)},
            { "time",    std::make_any<uint64_t>(time)},
            {"color", std::make_any<CHyprColor>(color)},
            { "icon",      std::make_any<eIcons>(icon)}
    });
}

} // namespace deco
