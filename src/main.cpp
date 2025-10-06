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
#include "log.hpp"
#include "plugin.hpp"

// Declared in plugin.hpp
UP<deco::Plugin> deco::g_plugin{nullptr};

using deco::g_plugin;

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
    g_plugin = makeUnique<deco::Plugin>(handle);
    g_plugin->init();
    deco::log("Initialization complete");
    g_plugin->notify("Initialized successfully.", ICON_OK);

    return {
        .name = "deco",
        .description = DECO_PLUGIN_DESCRIPTION,
        .author = DECO_PLUGIN_AUTHOR,
        .version = DECO_PLUGIN_VERSION};
}

APICALL EXPORT void PLUGIN_EXIT()
{
    TRACE;
    g_plugin.reset();
}
