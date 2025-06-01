#pragma once

#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <vector>

#include "bar/positioner.hpp"
#include "models/button.hpp"
#include "widgets/button.hpp"

class ButtonManager {
public:
    static ButtonManager newFromConfig(CBox const&);

    explicit ButtonManager(CBox const&);

    deco::ButtonInstance *getButtonAt(Vector2D const&);

    std::vector<deco::ButtonInstance>& buttons() { return m_buttons; }

    std::vector<deco::ButtonInstance> const& buttons() const
    {
        return m_buttons;
    }

    void addButton(deco::ButtonModel const&);
    void resize(CBox const&);

private:
    CBox m_box;
    Positioner m_positioner;
    std::vector<deco::ButtonInstance> m_buttons{};
};
