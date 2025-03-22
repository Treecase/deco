#include "bar.hpp"

#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include "bar/drawpass.hpp"
#include "models/bar.hpp"

using namespace deco::bar;

// Constructor

Bar::Bar(PHLWINDOW win)
: IHyprWindowDecoration{win}
, m_window{win}
{
}

// IHyprWindowDecoration Overrides

SDecorationPositioningInfo Bar::getPositioningInfo()
{
    static constexpr uint32_t HYPRLAND_BORDER_PRIORITY =
        10000; // Hardcoded in Hyprland.
    double const height = isHidden() ? 0 : g_barmodel.height();
    return {
        .policy = DECORATION_POSITION_STICKY,
        .edges = DECORATION_EDGE_TOP,
        .priority = HYPRLAND_BORDER_PRIORITY + 5,
        .desiredExtents = {{0.0, height}, {0, 0}},
        .reserved = true,
    };
}

void Bar::onPositioningReply(SDecorationPositioningReply const& reply)
{
    m_assignedBox = reply.assignedGeometry;
}

void Bar::draw(PHLMONITOR monitor, float const& a)
{
    auto const wants_decorations =
        m_window->m_sWindowData.decorate.valueOrDefault();
    if (!validMapped(m_window) || !wants_decorations || m_is_hidden) {
        return;
    }
    g_pHyprRenderer->m_sRenderPass.add(makeShared<RenderPass>(this));
}

eDecorationType Bar::getDecorationType()
{
    return DECORATION_CUSTOM;
}

void Bar::updateWindow(PHLWINDOW)
{
    damageEntire();
    g_pDecorationPositioner->repositionDeco(this);
}

void Bar::damageEntire()
{
    g_pHyprRenderer->damageBox(getFullRenderArea());
}

eDecorationLayer Bar::getDecorationLayer()
{
    return DECORATION_LAYER_UNDER;
}

uint64_t Bar::getDecorationFlags()
{
    return DECORATION_ALLOWS_MOUSE_INPUT | DECORATION_PART_OF_MAIN_WINDOW;
}

std::string Bar::getDisplayName()
{
    return "deco::Bar";
}
