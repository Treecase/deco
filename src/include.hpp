#pragma once

#include <format>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>

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

} // namespace deco
