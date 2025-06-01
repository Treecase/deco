#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/macros.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <hyprutils/string/VarList.hpp>
#include <string>

#include "defines.hpp"
#include "include.hpp"
#include "log.hpp"

// Declared in include.hpp
UP<deco::Plugin> deco::g_plugin{nullptr};

// WARNING: Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION()
{
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
#ifndef NDEBUG
    Debug::m_trace = true;
#endif

    TRACE;

    deco::log("Initializing...");
    deco::g_plugin = makeUnique<deco::Plugin>(handle);
    deco::g_plugin->init();
    deco::log("Initialization complete");
    deco::g_plugin->notify("Initialized successfully.", ICON_OK);

    return {
        .name = "deco",
        .description = DECO_PLUGIN_DESCRIPTION,
        .author = DECO_PLUGIN_AUTHOR,
        .version = DECO_PLUGIN_VERSION};
}

APICALL EXPORT void PLUGIN_EXIT()
{
    TRACE;
    deco::g_plugin.reset();
}
