#include <any>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/decorations/DecorationPositioner.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprutils/memory/UniquePtr.hpp>

#include "bar/bar.hpp"
#include "bar/drawpass.hpp"
#include "include.hpp"

using namespace deco::bar;

// IHyprWindowDecoration Overrides

SDecorationPositioningInfo Bar::getPositioningInfo()
{
    // Border priority is hardcoded in
    // hyprland/src/render/decorations/CHyprBorderDecoration.cpp.
    static constexpr uint32_t HYPRLAND_BORDER_PRIORITY = 10000;
    double const height = isHidden()
        ? 0
        : m_rule_height.value_or(g_plugin->config().bar.height());
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
    m_btnmgr.resize(m_assignedBox);
}

void Bar::draw(PHLMONITOR, float const&)
{
    if (isVisible()) {
        g_pHyprRenderer->m_renderPass.add(makeUnique<RenderPass>(this));
    }
}

eDecorationType Bar::getDecorationType()
{
    return DECORATION_CUSTOM;
}

void Bar::updateWindow(PHLWINDOW)
{
}

void Bar::damageEntire()
{
    g_pHyprRenderer->damageBox(
        getAssignedBoxInGlobalSpace().translate(m_window->m_floatingOffset));
}

bool Bar::onInputOnDeco(eInputType const, Vector2D const&, std::any)
{
    damageEntire();
    return true;
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
