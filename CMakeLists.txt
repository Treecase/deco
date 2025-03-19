cmake_minimum_required(VERSION 3.25)

project(deco
    DESCRIPTION "Window bars for Hyprland."
    VERSION "0.2"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)

add_library(deco SHARED
    src/bar/bar.cpp
    src/bar/barimpl.cpp
    src/bar/factory.cpp
    src/bar/drawpass.cpp
    src/config/keywords.cpp
    src/config/values.cpp
    src/widgets/button.cpp
    src/events.cpp
    src/main.cpp
)

set(DECO_AUTHOR "Treecase")
configure_file("src/defines.hpp.in" "defines.hpp")
target_include_directories(deco PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(deco PRIVATE "src")

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    libdrm
    libinput
    libudev
    pangocairo
    pixman-1
    wayland-server>=1.22.90
    wayland-protocols
    xkbcommon
)
target_link_libraries(deco PRIVATE rt PkgConfig::deps)
install(TARGETS deco)

