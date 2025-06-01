#pragma once

#include <hyprland/src/devices/IPointer.hpp>

enum eMouseButton : decltype(IPointer::SButtonEvent::button) {
    MOUSE_BUTTON_LEFT = 272,
    MOUSE_BUTTON_RIGHT = 273,
    MOUSE_BUTTON_MIDDLE = 274,
};
