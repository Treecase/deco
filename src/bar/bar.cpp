#include "bar/bar.hpp"

#include <wayland-server-protocol.h>

#include <any>
#include <functional>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/managers/SeatManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <memory>
#include <ranges>
#include <vector>

#include "bar/buttonmanager.hpp"
#include "include.hpp"
#include "input.hpp"
#include "log.hpp"
#include "widgets/button.hpp"

using deco::bar::Bar;

Bar::Bar(PHLWINDOW win)
: IHyprWindowDecoration{win}
, m_window{win}
, m_btnmgr{CBox{}}
{
    TRACE;
    deco::log("Register event callbacks for {}", m_window.lock());
    ptr_configReloaded = g_plugin->addCallback(
        "configReloaded",
        std::bind_front(&Bar::onConfigReloaded, this));
    ptr_mouseButton = g_plugin->addCallback(
        "mouseButton",
        std::bind_front(&Bar::onMouseButton, this));
    ptr_mouseMove = g_plugin->addCallback(
        "mouseMove",
        std::bind_front(&Bar::onMouseMove, this));
    ptr_windowUpdateRules = g_plugin->addCallback(
        "windowUpdateRules",
        std::bind_front(&Bar::onWindowUpdateRules, this));
    regenerateButtons();
}

void Bar::hide(bool hide)
{
    TRACE;
    if (hide == m_manual_hide) {
        return;
    }
    m_manual_hide = hide;
    deco::log(
        "Bar for {} manually {}",
        m_window.lock(),
        hide ? "hidden" : "unhidden");
    g_pDecorationPositioner->repositionDeco(this);
}

bool Bar::isHidden() const
{
    return isHiddenManual() || isHiddenRule();
}

bool Bar::isHiddenManual() const
{
    return m_manual_hide;
}

bool Bar::isHiddenRule() const
{
    return m_rule_hide;
}

bool Bar::isVisible() const
{
    return validMapped(m_window)
        && m_window->m_windowData.decorate.valueOrDefault()
        && !m_window->m_X11DoesntWantBorders && !isHidden();
}

CBox Bar::getAssignedBoxInGlobalSpace() const
{
    CBox box = m_assignedBox;
    box.translate(g_pDecorationPositioner->getEdgeDefinedPoint(
        DECORATION_EDGE_TOP,
        m_window.lock()));

    auto const workspace = m_window->m_workspace;
    if (!workspace) {
        return box;
    }
    auto const workspace_offset = (workspace && !m_window->m_pinned)
        ? workspace->m_renderOffset->value()
        : Vector2D();

    return box.translate(workspace_offset);
}

bool Bar::isMouseInside() const
{
    auto const window_at_cursor = g_pCompositor->vectorToWindowUnified(
        g_pInputManager->getMouseCoordsInternal(),
        RESERVED_EXTENTS | INPUT_EXTENTS | ALLOW_FLOATING);
    return window_at_cursor == m_window;
}

bool Bar::isEventValid() const
{
    if (!m_window->m_workspace || !m_window->m_workspace->isVisible()
        || !g_pInputManager->m_exclusiveLSes.empty()
        || (g_pSeatManager->m_seatGrab
            && !g_pSeatManager->m_seatGrab->accepts(
                m_window->m_wlSurface->resource()))) {
        return false;
    }
    auto const window_at_cursor = g_pCompositor->vectorToWindowUnified(
        g_pInputManager->getMouseCoordsInternal(),
        RESERVED_EXTENTS | INPUT_EXTENTS | ALLOW_FLOATING);
    if (!window_at_cursor != m_window
        && m_window != g_pCompositor->m_lastWindow) {
        return false;
    }
    std::vector layers{
        std::from_range,
        g_pCompositor->m_layers
            | std::views::transform(
                [](PHLLS layer) -> PHLLSREF { return PHLLSREF{layer}; })};
    Vector2D _coords{};
    PHLLS _layerSurface{};
    auto const layer = g_pCompositor->vectorToLayerSurface(
        g_pInputManager->getMouseCoordsInternal(),
        &layers,
        &_coords,
        &_layerSurface);
    if (layer) {
        deco::log("mouse is over a layer surface");
        return false;
    }
    return true;
}

Vector2D Bar::getMouseRelative() const
{
    return getGlobalPointRelative(g_pInputManager->getMouseCoordsInternal());
}

Vector2D Bar::getGlobalPointRelative(Vector2D const& point) const
{
    auto const box = getAssignedBoxInGlobalSpace();
    return point - box.pos();
}

double Bar::getCenterline() const
{
    return m_assignedBox.height / 2.0;
}

void Bar::regenerateButtons()
{
    TRACE;
    std::construct_at(&m_btnmgr, ButtonManager::newFromConfig(m_assignedBox));
}

void Bar::onConfigReloaded(void *, SCallbackInfo&, std::any)
{
    regenerateButtons();
}

// NOTE: This is called for _all_ button events, not just clicks inside
// the bar itself.
void Bar::onMouseButton(void *, SCallbackInfo&, std::any data)
{
    TRACE;
    auto const btn = std::any_cast<IPointer::SButtonEvent>(data);
    deco::log(
        "Bar::onMouseButton: btn {}",
        btn.state == WL_POINTER_BUTTON_STATE_PRESSED ? "down" : "up");

    // We only care about left clicks
    if (btn.button != MOUSE_BUTTON_LEFT) {
        return;
    }

    auto const button_is_down = btn.state == WL_POINTER_BUTTON_STATE_PRESSED;
    auto const pointer = getGlobalPointRelative(m_mouse_pos);
    auto const pointer_is_inside_bar =
        getAssignedBoxInGlobalSpace().containsPoint(m_mouse_pos)
        && isEventValid();

    if (pointer_is_inside_bar) {
        auto btn = m_btnmgr.getButtonAt(pointer);
        if (btn != nullptr) {
            // A button was clicked
            btn->setState(State::CLICKED);
            deco::log("Clicked button {}", btn->name());
            damageEntire();
        } else {
            // Click on the bar itself
            if (button_is_down) {
                // Button clicked, start of a drag
                m_drag.emplace(m_mouse_pos);
            } else {
                // Button released, end of a drag
                m_drag.reset();
            }
        }
    } else {
        // Click occurred outside of the bar
        if (m_drag.has_value() && !button_is_down) {
            // Button released, end of a drag
            m_drag.reset();
        }
    }

    // Check for released buttons
    if (btn.state == WL_POINTER_BUTTON_STATE_RELEASED) {
        auto clicked_buttons =
            m_btnmgr.buttons() | std::views::filter([](auto& b) {
                return b.state() == State::CLICKED;
            });
        for (auto& button : clicked_buttons) {
            deco::log("Released button {}", button.name());
            button.setState(State::NORMAL);
            if (button.box().containsPoint(pointer)) {
                button.model().exec();
            }
        }
        damageEntire();
    }
}

void Bar::onMouseMove(void *, SCallbackInfo&, std::any data)
{
    TRACE;
    m_mouse_pos = std::any_cast<Vector2D>(data);

    auto const pointer = getGlobalPointRelative(m_mouse_pos);
    auto btn = m_btnmgr.getButtonAt(pointer);

    if (m_drag.has_value()) {
        // Window is being dragged
        g_pKeybindManager->m_dispatchers["mouse"]("1movewindow");
    } else if (btn != nullptr) {
        // Mouse is over a button
        deco::log("Hovered button {}", btn->name());
        if (btn->state() != State::CLICKED) {
            btn->setState(State::HOVERED);
        }
        damageEntire();
    } else {
        // Mouse is somewhere else
        for (auto& button : m_btnmgr.buttons()) {
            if (button.state() == State::HOVERED) {
                button.setState(State::NORMAL);
            }
        }
        damageEntire();
    }
}

void Bar::onWindowUpdateRules(void *, SCallbackInfo&, std::any data)
{
    TRACE;
    PHLWINDOW const window = std::any_cast<PHLWINDOW>(data);
    PHLWINDOW const bar_win = m_window.lock();
    if (bar_win != window) {
        return;
    }
    auto const it = std::ranges::find_if(
        bar_win->m_matchedRules,
        [](auto rule) { return rule->m_rule == "plugin:deco:nobar"; });
    auto const matched = it != bar_win->m_matchedRules.end();
    if (matched) {
        deco::log("{} matched rule plugin:deco:nobar", window);
    }
    m_rule_hide = matched;
    g_pDecorationPositioner->repositionDeco(this);
    bar_win->updateWindowDecos();
}

// Drag ///////////////////////////////////////////////////////////////////////

Bar::Drag::Drag(Vector2D const& start)
: start{start}
{
    deco::log("Drag {} started at {}", static_cast<void *>(this), start);
    g_pKeybindManager->m_dispatchers["mouse"]("1movewindow");
}

Bar::Drag::~Drag()
{
    g_pKeybindManager->m_dispatchers["mouse"]("0movewindow");
    deco::log("Drag {} ended", static_cast<void *>(this));
}
