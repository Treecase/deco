#pragma once

#include <hyprland/src/defines.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <string>

#include "models/button.hpp"

namespace deco {

using State = deco::ButtonState;

class ButtonInstance {
public:
    explicit ButtonInstance(ButtonModel const& model, Vector2D const& position);

    std::string name() const { return m_model.name(); }

    CBox box() const;
    deco::ButtonModel const& model() const;
    Vector2D position() const;
    State state() const;

    void setState(State const&);
    void setPosition(Vector2D const&);

private:
    deco::ButtonModel const& m_model;
    Vector2D m_position; // Relative to bar position
    State m_state{State::NORMAL};
    bool m_is_mouse_inside{false};
};

} // namespace deco
