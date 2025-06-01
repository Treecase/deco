#pragma once

#include <hyprlang.hpp>
#include <hyprutils/math/Box.hpp>

#include "models/button.hpp"

class Positioner {
public:
    explicit Positioner(CBox const& bounds);

    Hyprutils::Math::CBox position(deco::ButtonModel const&);

private:
    Hyprlang::INT x_offset;
    CBox const bounds;
};
