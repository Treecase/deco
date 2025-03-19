#include "events.hpp"

#include <cassert>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "models/bar.hpp"

//

static SP<HOOK_CALLBACK_FN> ptr_preConfigReload{nullptr};
static void cb_preConfigReload(void *self, SCallbackInfo& info, std::any data);

///////////////////////////////////////////////////////////////////////////////

void deco::events::init()
{
    deco::log("Register event callbacks");
    ptr_preConfigReload = HyprlandAPI::registerCallbackDynamic(
        deco::g_handle,
        "preConfigReload",
        cb_preConfigReload);
    assert(ptr_preConfigReload);
}

//// EVENT CALLBACKS //////////////////////////////////////////////////////////

static void cb_preConfigReload(void *self, SCallbackInfo& info, std::any data)
{
    deco::log("Config reloading...");
    deco::g_barmodel.buttons.clear();
}
