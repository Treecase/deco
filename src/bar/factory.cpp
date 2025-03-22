#include <cassert>
#include <expected>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "bar.hpp"
#include "bar/bar.hpp"
#include "include.hpp"

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

static void cb_windowUpdateRules(
    void *self,
    SCallbackInfo& info,
    std::any data,
    deco::bar::Bar *bar)
{
    auto const window = std::any_cast<PHLWINDOW>(data);
    deco::log("windowUpdateRules {}", window);

    auto const bar_win = bar->m_window.lock();
    if (bar_win != window) {
        deco::log("bar_win {} is not {}", bar_win, window);
        return;
    }

    deco::log("Window rules updated for {}", window);

    auto const rules = bar_win->m_vMatchedRules;
    for (auto const& rule : rules) {
        if (rule->szRule == "plugin:deco:nobar") {
            bar->hide(!bar->isHidden());
        }
    }

    window->updateWindowDecos();
}

struct deco::bar::EventCallbacks {
    SP<HOOK_CALLBACK_FN> ptr_mouseButton;
    SP<HOOK_CALLBACK_FN> ptr_mouseMove;
    SP<HOOK_CALLBACK_FN> ptr_windowUpdateRules;

    EventCallbacks(Bar *bar)
    {
        deco::log("Register event callbacks for {}", bar->m_window.lock());
        ptr_mouseButton = HyprlandAPI::registerCallbackDynamic(
            deco::g_handle,
            "mouseButton",
            [bar](auto a, auto b, auto c) { cb_mouseButton(a, b, c, bar); });
        ptr_mouseMove = HyprlandAPI::registerCallbackDynamic(
            deco::g_handle,
            "mouseMove",
            [bar](auto a, auto b, auto c) { cb_mouseMove(a, b, c, bar); });
        ptr_windowUpdateRules = HyprlandAPI::registerCallbackDynamic(
            deco::g_handle,
            "windowUpdateRules",
            [bar](auto a, auto b, auto c) {
                cb_windowUpdateRules(a, b, c, bar);
            });
        assert(ptr_mouseButton);
        assert(ptr_mouseMove);
        assert(ptr_windowUpdateRules);
    }
};

//// FACTORY //////////////////////////////////////////////////////////////////

using namespace deco::bar;

static void
cb_openWindow(void *self, SCallbackInfo& info, std::any data, Factory *factory)
{
    using Err = Factory::CreateForError;

    auto const window = std::any_cast<PHLWINDOW>(data);
    deco::log("New window {}", window);

    auto const e = factory->createFor(window);
    if (e.error_or(Err::DoesntWantDecorations) != Err::DoesntWantDecorations) {
        deco::notify(
            std::format("Failed to create deco for {}", window),
            ICON_ERROR);
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

std::expected<WP<Bar>, Factory::CreateForError> Factory::createFor(
    PHLWINDOW window)
{
    if (window->m_bX11DoesntWantBorders) {
        deco::log("{} doesnt want decorations", window);
        return std::unexpected{CreateForError::DoesntWantDecorations};
    }
    deco::log("Create new Bar for window {}", window);

    auto bar = makeUnique<deco::bar::Bar>(window);
    bar->cbs = makeUnique<EventCallbacks>(bar.get());

    WP<Bar> wp{bar};

    auto const success = HyprlandAPI::addWindowDecoration(
        deco::g_handle,
        window,
        std::move(bar));
    if (success) {
        return wp;
    }
    return std::unexpected{CreateForError::Failed};
}
