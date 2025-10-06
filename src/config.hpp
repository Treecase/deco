#pragma once

#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>

#define CFGVAL(Ns, Name, Type, Default)                    \
    namespace Ns {                                         \
    struct Name {                                          \
        using type = Type;                                 \
        static type constexpr DEFAULT = Default;           \
        static constexpr char const *NAME = #Ns ":" #Name; \
    };                                                     \
    }

namespace deco::config {

enum Side : Hyprlang::INT {
    LEFT = 0,
    RIGHT = 1
};

CFGVAL(bar, height, Hyprlang::INT, 32)
CFGVAL(bar, fill_color, Hyprlang::INT, 0xff11111b)
CFGVAL(bar, text_enabled, Hyprlang::INT, 1)
CFGVAL(bar, text_color, Hyprlang::INT, 0xffcdd6f4)
CFGVAL(bar, text_size, Hyprlang::INT, 12)

CFGVAL(buttons, side, Hyprlang::INT, Side::RIGHT)
CFGVAL(buttons, spacing, Hyprlang::INT, 8)
CFGVAL(buttons, padding, Hyprlang::INT, 8)
CFGVAL(buttons, diameter, Hyprlang::INT, 22)

} // namespace deco::config

#undef CFGVAL
