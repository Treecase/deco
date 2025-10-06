#include "bar/buttonmanager.hpp"

#include <algorithm>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <optional>
#include <ranges>
#include <vector>

#include "config.hpp"
#include "include.hpp"
#include "log.hpp"
#include "models/bar.hpp"
#include "widgets/button.hpp"

using deco::config::Side;

ButtonManager ButtonManager::newFromConfig(CBox const& box)
{
    TRACE;
    ButtonManager btnmgr{box};
    for (auto const& btn : deco::g_plugin->barModel().buttons()) {
        btnmgr.addButton(btn);
    }
    return btnmgr;
}

ButtonManager::ButtonManager(CBox const& box)
: m_box{box}
{
    TRACE;
}

std::optional<deco::ButtonInstance *> ButtonManager::getButtonAt(
    Vector2D const& position)
{
    TRACE;
    auto const it = std::ranges::find_if(m_buttons, [&](auto const& btn) {
        return btn.box().containsPoint(position);
    });
    if (it != std::ranges::end(m_buttons)) {
        return std::make_optional(it.base());
    }
    return std::nullopt;
}

void ButtonManager::addButton(deco::ButtonModel const& btn)
{
    m_buttons.emplace_back(btn, Vector2D{});
    updateLayout();
}

void ButtonManager::resize(CBox const& box)
{
    TRACE;
    m_box = box;
    updateLayout();
}

void ButtonManager::updateLayout()
{
    int const BUTTON_SIZE = deco::g_plugin->config().buttons.diameter();
    int const PADDING = deco::g_plugin->config().buttons.padding();
    int const SPACING = deco::g_plugin->config().buttons.spacing();
    Side const SIDE = deco::g_plugin->config().buttons.side();

    double const y = 0.5 * (m_box.height - BUTTON_SIZE);
    for (auto [i, btn] : std::views::enumerate(m_buttons)) {
        double const x = PADDING + ((i + 1) * BUTTON_SIZE) + (i * SPACING);
        if (SIDE == Side::LEFT) {
            btn.setPosition(Vector2D{x, y});
        } else {
            btn.setPosition(Vector2D{m_box.width - x, y});
        }
    }
}
