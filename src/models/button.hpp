#pragma once

#include <format>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>

#include "include.hpp"

namespace deco {

struct ButtonModel {
    int diameter{16};
    CHyprColor normal{0xff45475a};
    CHyprColor hovered{0xff585b70};
    CHyprColor clicked{0xff313244};
    std::string action{};

    Hyprutils::Math::Vector2D size() const { return {diameter, diameter}; }

    Hyprutils::Math::CBox box() const
    {
        return {
            {0, 0},
            size()
        };
    }

    void exec() const
    {
        deco::log("exec button action '{}'", action);
        auto dispatch_exec = g_pKeybindManager->m_mDispatchers.at("exec");
        std::invoke(dispatch_exec, action);
    }
};

} // namespace deco

template<>
struct std::formatter<deco::ButtonModel> {
    template<class FmtContext>
    FmtContext::iterator format(deco::ButtonModel const& btn, FmtContext& ctx)
        const
    {
        auto const out = std::format(
            "Button(diameter={}  clicked={:x}  hovered={:x}  normal={:x}  "
            "action={})",
            btn.diameter,
            btn.clicked.getAsHex(),
            btn.hovered.getAsHex(),
            btn.normal.getAsHex(),
            btn.action);
        return std::ranges::copy(std::move(out), ctx.out()).out;
    }

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format args for ButtonModel.");
        } else {
            return it;
        }
    }
};
