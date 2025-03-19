#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <hyprutils/string/VarList.hpp>
#include <string>

#include "bar/bar.hpp"
#include "config/config.hpp"
#include "defines.hpp"
#include "events.hpp"
#include "include.hpp"

// WARNING: Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION()
{
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
    deco::g_handle = handle;

    deco::log("Initializing...");

    // ALWAYS add this to your plugins. It will prevent random crashes coming
    // from mismatched header versions.
    std::string const HASH = __hyprland_api_get_hash();
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotificationV2(
            deco::g_handle,
            {
                { "text", "[deco] Mismatched headers! Can't proceed."},
                { "time",                                        5000},
                {"color",                                           0},
                { "icon",                                  ICON_ERROR}
        });
        throw std::runtime_error("[deco] Version mismatch");
    }

    deco::config::init();
    deco::events::init();

    deco::log("Add bars to existing windows");
    for (auto w : g_pCompositor->m_vWindows) {
        try {
            deco::bar::g_factory.createFor(w);
        } catch (std::runtime_error const& e) {
            HyprlandAPI::addNotificationV2(
                deco::g_handle,
                {
                    { "text",   e.what()},
                    { "time",       5000},
                    {"color",          0},
                    { "icon", ICON_ERROR}
            });
        }
    }

    deco::log("Initialization complete");
    HyprlandAPI::addNotificationV2(
        deco::g_handle,
        {
            { "text", "[deco] Initialized successfully."},
            { "time",                               5000},
            {"color",                                  0},
            { "icon",                            ICON_OK}
    });

    return {
        .name = "deco",
        .description = DECO_PLUGIN_DESCRIPTION,
        .author = DECO_PLUGIN_AUTHOR,
        .version = DECO_PLUGIN_VERSION};
}

APICALL EXPORT void PLUGIN_EXIT()
{
}
