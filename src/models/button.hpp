#pragma once

#include <format>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprlang.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <string>

#include "log.hpp"

namespace deco {

enum class ButtonState {
    NORMAL,
    HOVERED,
    CLICKED
};

class ButtonModel {
public:
    ButtonModel(
        std::string const& action,
        Hyprlang::INT diameter,
        CHyprColor const& normal,
        CHyprColor const& hovered,
        CHyprColor const& clicked);

    Vector2D size() const;
    CBox box() const;
    void exec() const;

    std::string name() const { return m_action; }

    CHyprColor const& colorFor(ButtonState const& state) const;

private:
    std::string const m_action;
    Hyprlang::INT const m_diameter;
    CHyprColor const m_normal;
    CHyprColor const m_hovered;
    CHyprColor const m_clicked;

    friend struct std::formatter<deco::ButtonModel>;
};

} // namespace deco

template<>
struct std::formatter<deco::ButtonModel> {
    template<class FmtContext>
    FmtContext::iterator format(deco::ButtonModel const& btn, FmtContext& ctx)
        const
    {
        auto const out = std::format(
            "Button(action={} diameter={} clicked={:x} hovered={:x} "
            "normal={:x})",
            btn.m_action,
            btn.m_diameter,
            btn.m_clicked.getAsHex(),
            btn.m_hovered.getAsHex(),
            btn.m_normal.getAsHex());
        return std::ranges::copy(std::move(out), ctx.out()).out;
    }

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            deco::panic("Invalid format args for ButtonModel.");
        } else {
            return it;
        }
    }
};
