#include "bar/buttonmanager.hpp"

#include <hyprutils/math/Box.hpp>
#include <memory>
#include <vector>

#include "bar/positioner.hpp"
#include "include.hpp"
#include "log.hpp"
#include "models/bar.hpp"
#include "widgets/button.hpp"

using deco::ButtonInstance;

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
, m_positioner{m_box}
{
    TRACE;
}

deco::ButtonInstance *ButtonManager::getButtonAt(Vector2D const& position)
{
    TRACE;
    auto const it = std::ranges::find_if(m_buttons, [&position](auto& btn) {
        return btn.box().containsPoint(position);
    });
    if (it != m_buttons.end()) {
        auto& btn = *it;
        return &btn;
    } else {
        return nullptr;
    }
}

void ButtonManager::addButton(deco::ButtonModel const& btn)
{
    auto const box = m_positioner.position(btn);
    m_buttons.push_back(ButtonInstance{btn, box.pos()});
}

void ButtonManager::resize(CBox const& box)
{
    TRACE;
    m_box = box;
    // std::destroy_at(&m_positioner);
    std::construct_at(&m_positioner, Positioner{m_box});
    for (auto& btn : m_buttons) {
        auto const box = m_positioner.position(btn.model());
        btn.setPosition(box.pos());
    }
}
