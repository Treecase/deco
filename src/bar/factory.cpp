#include <hyprland/src/desktop/Window.hpp>

#include "bar/bar.hpp"
#include "include.hpp"

using namespace deco::bar;

WP<Bar> Factory::createFor(PHLWINDOW window)
{
    deco::log("Create new Bar for window {}", window);
    auto bar = makeUnique<deco::bar::Bar>(window);
    auto const wp = m_bars.emplace_back(bar);

    if (!HyprlandAPI::addWindowDecoration(
            deco::g_handle,
            window,
            std::move(bar))) {
        throw std::runtime_error{
            std::format("Failed to create deco for {}", window)};
    }
    return wp;
}
