#pragma once

#include <hyprland/src/defines.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/SharedDefs.hpp>

#include "models/button.hpp"

namespace widget {

enum State {
    NORMAL,
    HOVERED,
    CLICKED,
};

class Button {
public:
    deco::ButtonModel const& model;
    Vector2D position; // Relative to bar position
    State state{NORMAL};

    Button(deco::ButtonModel const&, Vector2D const&);
    CBox box() const;

    void render(Vector2D const& origin, double scale) const;
};

} // namespace widget
