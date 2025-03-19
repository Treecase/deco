#pragma once

#include <hyprland/src/helpers/Color.hpp>

#include "config/config.hpp"
#include "models/button.hpp"

namespace deco {

struct BarModel {
    auto height() const { return config::bar::height::get(); }

    CHyprColor fill() const { return config::bar::fill_color::get(); }

    std::vector<ButtonModel> buttons{};
};

inline BarModel g_barmodel{};

} // namespace deco
