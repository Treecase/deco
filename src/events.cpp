#include "events.hpp"

#include <cassert>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "bar/bar.hpp"
#include "include.hpp"
#include "models/bar.hpp"

#define DECLARE_CB(_event)                                                   \
    static SP<HOOK_CALLBACK_FN> ptr_##_event{nullptr};                       \
    static void cb_##_event(void *self, SCallbackInfo& info, std::any data);

#define REGISTER_CB(_event)                              \
    ptr_##_event = HyprlandAPI::registerCallbackDynamic( \
        deco::g_handle,                                  \
        #_event,                                         \
        cb_##_event);                                    \
    assert(ptr_##_event);

//

DECLARE_CB(openWindow)
DECLARE_CB(mouseButton)
DECLARE_CB(mouseMove)
DECLARE_CB(preConfigReload)

///////////////////////////////////////////////////////////////////////////////

void deco::events::init()
{
    deco::log("Register event callbacks");
    REGISTER_CB(openWindow);
    REGISTER_CB(mouseButton);
    REGISTER_CB(mouseMove);
    REGISTER_CB(preConfigReload);
}

//// EVENT CALLBACKS //////////////////////////////////////////////////////////

static void cb_openWindow(void *self, SCallbackInfo& info, std::any data)
{
    auto window = std::any_cast<PHLWINDOW>(data);
    deco::log("New window {}", window);
    deco::bar::g_factory.createFor(window);
}

static void cb_mouseButton(void *self, SCallbackInfo& info, std::any data)
{
    auto const event = std::any_cast<IPointer::SButtonEvent>(data);
    for (auto bar : deco::bar::g_factory.bars()) {
        info.cancelled |= bar->onMouseButton(event);
    }
}

static void cb_mouseMove(void *self, SCallbackInfo& info, std::any data)
{
    auto const mouse_pos = std::any_cast<Vector2D>(data);
    for (auto bar : deco::bar::g_factory.bars()) {
        info.cancelled |= bar->onMouseMove(mouse_pos);
    }
}

static void cb_preConfigReload(void *self, SCallbackInfo& info, std::any data)
{
    deco::log("Config reloading...");
    deco::g_barmodel.buttons.clear();
}
