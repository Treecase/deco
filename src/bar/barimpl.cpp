#include <cassert>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include "bar/bar.hpp"
#include "config/config.hpp"
#include "include.hpp"
#include "widgets/button.hpp"
#include "widgets/container.hpp"

using namespace deco::bar;

// Event Handlers

bool Bar::onMouseButton(IPointer::SButtonEvent event)
{
    m_was_clicked = false;
    auto const pointer = getMouseRelative();

    // Check if the user has clicked on a button.
    for (auto const& btn : positionButtons(m_assignedBox.size())) {
        auto btn_box = btn.box();
        bool const is_hovered = btn_box.containsPoint(pointer);
        if (is_hovered && event.state == WL_POINTER_BUTTON_STATE_PRESSED) {
            m_was_button_clicked = true;
            g_pHyprRenderer->damageBox(
                btn_box.translate(getAssignedBoxInGlobalSpace().pos()));
            m_last_click_pos = pointer;
            btn.model.exec();
            return false;
        }
    }

    switch (event.state) {
    case WL_POINTER_BUTTON_STATE_PRESSED:
        if (isMouseInside()) {
            m_was_clicked = true;
            return true;
        }
        break;
    case WL_POINTER_BUTTON_STATE_RELEASED:
        if (m_was_button_clicked) {
            damageEntire();
        }
        m_was_button_clicked = false;
        if (m_is_dragged) {
            endDrag();
        }
        break;
    }
    return false;
}

bool Bar::onMouseMove(Vector2D pointer_global)
{
    auto const pointer = getGlobalPointRelative(pointer_global);

    // User is dragging the window
    if (m_is_dragged) {
        updateDrag();
    }
    // User is initiating a drag.
    else if (m_was_clicked) {
        startDrag();
    } else if (!m_was_button_clicked) {
        // Check if the mouse has entered or exited a button, and redraw it if
        // so.
        for (auto const& btn : positionButtons(m_assignedBox.size())) {
            auto btn_box = btn.box();
            bool const is_hovered = btn_box.containsPoint(pointer);
            bool const was_hovered = btn_box.containsPoint(m_last_mouse_pos);
            if (is_hovered != was_hovered) {
                g_pHyprRenderer->damageBox(
                    btn_box.translate(getAssignedBoxInGlobalSpace().pos()));
            }
        }
    }

    m_last_mouse_pos = pointer;
    return false;
}

//

void Bar::render() const
{
    auto const mon = g_pHyprOpenGL->m_RenderData.pMonitor;
    auto const win = m_window;

    auto const rounding = m_window->rounding() * mon->scale;

    // Bar rect in render space.
    auto barrect = getFullRenderArea()
                       .translate(-mon->vecPosition)
                       .translate(m_window->m_vFloatingOffset)
                       .scale(mon->scale)
                       .round();

    // Draw the bar.
    g_pHyprOpenGL->renderRect(
        barrect,
        config::bar::fill_color::get(),
        rounding,
        win->roundingPower());

    // Draw the buttons.
    for (auto btn : deco::positionButtons(m_assignedBox.size())) {
        if (btn.box().containsPoint(getMouseRelative())) {
            btn.state =
                m_was_button_clicked ? widget::CLICKED : widget::HOVERED;
        }
        btn.render(barrect.pos(), mon->scale);
    }
}

//

void Bar::startDrag()
{
    deco::log("Drag started on {}", m_window.lock());
    g_pKeybindManager->m_mDispatchers["mouse"]("0movewindow");
    m_is_dragged = true;
}

void Bar::updateDrag()
{
    assert(m_is_dragged);
    g_pKeybindManager->m_mDispatchers["mouse"]("1movewindow");
}

void Bar::endDrag()
{
    m_is_dragged = false;
    g_pKeybindManager->m_mDispatchers["mouse"]("0movewindow");
    deco::log("Drag ended on {}", m_window.lock());
}

void Bar::hide(bool hide)
{
    m_is_hidden = hide;
    deco::log("Bar for {} {}", m_window.lock(), hide ? "hidden" : "unhidden");
    g_pDecorationPositioner->repositionDeco(this);
}

// Private Helpers

CBox Bar::getAssignedBoxInGlobalSpace() const
{
    CBox box = m_assignedBox;
    box.translate(g_pDecorationPositioner->getEdgeDefinedPoint(
        DECORATION_EDGE_TOP,
        m_window.lock()));

    auto const workspace = m_window->m_pWorkspace;
    if (!workspace) {
        return box;
    }
    auto const workspace_offset = (workspace && !m_window->m_bPinned)
        ? workspace->m_vRenderOffset->value()
        : Vector2D();

    return box.translate(workspace_offset);
}

CBox Bar::getFullRenderArea() const
{
    auto box = getAssignedBoxInGlobalSpace();
    box.height += m_window->rounding();
    return box;
}

bool Bar::isMouseInside() const
{
    auto const cursor_pos = g_pInputManager->getMouseCoordsInternal();
    auto const box = getAssignedBoxInGlobalSpace();
    return box.containsPoint(cursor_pos);
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
