#pragma once

#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <optional>
#include <vector>

#include "models/button.hpp"
#include "widgets/button.hpp"

class ButtonManager {
public:
    static ButtonManager newFromConfig(CBox const&);

    explicit ButtonManager(CBox const&);

    std::optional<deco::ButtonInstance *> getButtonAt(Vector2D const&);

    std::vector<deco::ButtonInstance>& buttons() { return m_buttons; }

    std::vector<deco::ButtonInstance> const& buttons() const
    {
        return m_buttons;
    }

    void addButton(deco::ButtonModel const&);
    void resize(CBox const&);

private:
    CBox m_box;
    std::vector<deco::ButtonInstance> m_buttons{};

    void updateLayout();
};
