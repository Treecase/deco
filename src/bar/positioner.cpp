#include "bar/positioner.hpp"

#include <hyprlang.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>

#include "config.hpp"
#include "include.hpp"
#include "log.hpp"
#include "models/button.hpp"

using deco::ButtonModel;
using deco::g_plugin;
using deco::config::Side;

Positioner::Positioner(CBox const& bounds)
: x_offset{g_plugin->config().buttons.padding()}
, bounds{bounds}
{
}

CBox Positioner::position(ButtonModel const& btn)
{
    TRACE;
    auto const doPosition = [this](CBox const& box) -> double {
        switch (g_plugin->config().buttons.side()) {
        case Side::LEFT:
            return x_offset;
        default:
            return bounds.width - x_offset - box.width;
        }
    };
    Vector2D const pos{
        doPosition(btn.box()),
        0.5 * (bounds.height - btn.box().height)};
    x_offset += btn.box().width + g_plugin->config().buttons.spacing();
    auto const box = btn.box().translate(pos);
    return box;
}
