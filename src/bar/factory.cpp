#include <cassert>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "bar.hpp"
#include "bar/bar.hpp"
#include "events.hpp"
#include "include.hpp"
#include "models/bar.hpp"

//// EVENT CALLBACKS //////////////////////////////////////////////////////////

static void cb_mouseButton(
    void *self,
    SCallbackInfo& info,
    std::any data,
    deco::bar::Bar *bar)
{
    auto const event = std::any_cast<IPointer::SButtonEvent>(data);
    info.cancelled = bar->onMouseButton(event);
}

static void cb_mouseMove(
    void *self,
    SCallbackInfo& info,
    std::any data,
    deco::bar::Bar *bar)
{
    auto const mouse_pos = std::any_cast<Vector2D>(data);
    info.cancelled = bar->onMouseMove(mouse_pos);
}

struct deco::bar::EventCallbacks {
    SP<HOOK_CALLBACK_FN> ptr_mouseButton;
    SP<HOOK_CALLBACK_FN> ptr_mouseMove;

    EventCallbacks(Bar *bar)
    {
        ptr_mouseButton = HyprlandAPI::registerCallbackDynamic(
            deco::g_handle,
            "mouseButton",
            [bar](auto a, auto b, auto c) { cb_mouseButton(a, b, c, bar); });
        ptr_mouseMove = HyprlandAPI::registerCallbackDynamic(
            deco::g_handle,
            "mouseMove",
            [bar](auto a, auto b, auto c) { cb_mouseMove(a, b, c, bar); });
        assert(ptr_mouseButton);
        assert(ptr_mouseMove);
    }
};

//// FACTORY //////////////////////////////////////////////////////////////////

#include <hyprland/src/desktop/Window.hpp>

#include "bar/bar.hpp"
#include "include.hpp"

using namespace deco::bar;

static void
cb_openWindow(void *self, SCallbackInfo& info, std::any data, Factory *factory)
{
    auto window = std::any_cast<PHLWINDOW>(data);
    deco::log("New window {}", window);
    try {
        factory->createFor(window);
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

Factory::Factory()
{
    ptr_openWindow = HyprlandAPI::registerCallbackDynamic(
        deco::g_handle,
        "openWindow",
        [this](auto a, auto b, auto c) { cb_openWindow(a, b, c, this); });
    assert(ptr_openWindow);
}

WP<Bar> Factory::createFor(PHLWINDOW window)
{
    deco::log("Create new Bar for window {}", window);

    auto bar = makeUnique<deco::bar::Bar>(window);
    bar->cbs = makeUnique<EventCallbacks>(bar.get());

    WP<Bar> const wp{bar};

    auto const success = HyprlandAPI::addWindowDecoration(
        deco::g_handle,
        window,
        std::move(bar));
    if (success) {
        return wp;
    }
    throw std::runtime_error{
        std::format("Failed to create deco for {}", window)};
}
