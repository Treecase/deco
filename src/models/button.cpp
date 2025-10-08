#include "models/button.hpp"

#include <functional>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <stdexcept>
#include <string>

#include "log.hpp"

using deco::ButtonModel;

ButtonModel::ButtonModel(
    std::string const& action,
    Hyprlang::INT diameter,
    CHyprColor const& normal,
    CHyprColor const& hovered,
    CHyprColor const& clicked)
: m_action{action}
, m_diameter{diameter}
, m_normal{normal}
, m_hovered{hovered}
, m_clicked{clicked}
{
    TRACE;
}

Vector2D ButtonModel::size() const
{
    auto const diameter = static_cast<int>(m_diameter);
    return {diameter, diameter};
}

CBox ButtonModel::box() const
{
    return CBox{
        {0, 0},
        size()
    };
}

void ButtonModel::exec() const
{
    deco::log("Executing button action \"{}\"", m_action);
    auto const dispatch_exec = g_pKeybindManager->m_dispatchers.at("exec");
    std::invoke(dispatch_exec, m_action);
}

CHyprColor const& ButtonModel::colorFor(ButtonState const& state) const
{
    switch (state) {
    case ButtonState::NORMAL:
        return m_normal;
    case ButtonState::HOVERED:
        return m_hovered;
    case ButtonState::CLICKED:
        return m_clicked;
    }
    panic("invalid ButtonState");
}
