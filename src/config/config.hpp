#pragma once

#include <hyprlang.hpp>

#define DECO_VALUEMACRO 1
#include "helpers.hpp"
#undef DECO_VALUEMACRO

namespace deco::config {

VALUE(bar, height, Hyprlang::INT, 32);
VALUE(bar, fill_color, Hyprlang::INT, 0xff11111b);
VALUE(buttons, side, Hyprlang::STRING, "right");
VALUE(buttons, spacing, Hyprlang::INT, 8);
VALUE(buttons, padding, Hyprlang::INT, 8);
#undef VALUE

void init_values();
void init_keywords();

inline static void init()
{
    deco::log("Initializing config");
    init_values();
    init_keywords();
    HyprlandAPI::reloadConfig();
}

} // namespace deco::config
