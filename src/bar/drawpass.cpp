#include "drawpass.hpp"

#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/render/Renderer.hpp>

// Constructor

deco::bar::RenderPass::RenderPass(Bar *bar)
: m_bar{bar}
{
}

// IPassElement Overrides

void deco::bar::RenderPass::draw(CRegion const&)
{
    m_bar->render();
}

bool deco::bar::RenderPass::needsLiveBlur()
{
    return false;
}

bool deco::bar::RenderPass::needsPrecomputeBlur()
{
    return false;
}

char const *deco::bar::RenderPass::passName()
{
    return "deco::RenderPass";
}
