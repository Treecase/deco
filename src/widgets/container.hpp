#pragma once

#include <wayland-server-protocol.h>

#include <hyprland/src/includes.hpp>
#include <vector>

#include "config/config.hpp"
#include "models/bar.hpp"
#include "widgets/button.hpp"

namespace deco {

#define DIRECTION getDirection()
#define PADDING   deco::config::get<deco::config::buttons::padding>()
#define SPACING   deco::config::get<deco::config::buttons::spacing>()

/// Direction the container expands as children are added.
enum Direction { LTR, RTL };

inline static Direction getDirection()
{
    std::string const side{deco::config::buttons::side::get()};
    if (side == "left") {
        return LTR;
    } else if (side == "right") {
        return RTL;
    } else {
        throw std::runtime_error{std::format(
            "[deco] Invalid buttons:side value \"{}\", defaulting to \"right\"",
            side)};
    }
}

inline static std::vector<widget::Button> positionButtons(Vector2D const& size)
{
    std::vector<widget::Button> out{};
    double x_offset = PADDING;
    for (auto const& btn : g_barmodel.buttons) {
        Vector2D const position{
            DIRECTION == LTR ? x_offset : size.x - x_offset - btn.diameter,
            0.5 * size.y - 0.5 * btn.diameter};
        out.emplace_back(btn, position);
        x_offset += btn.diameter + SPACING;
    }
    return out;
}

#undef SPACING
#undef PADDING
#undef DIRECTION

} // namespace deco
