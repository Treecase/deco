#include "drawpass.hpp"

#include <GLES3/gl32.h>

#include <algorithm>
#include <hyprland/src/debug/Log.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/macros.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>

#include "log.hpp"
#include "models/button.hpp"
#include "plugin.hpp"
#include "widgets/button.hpp"

using deco::ButtonInstance;
using deco::bar::RenderPass;

RenderPass::RenderPass(Bar const *bar)
: m_bar{bar}
{
}

void RenderPass::renderBar(int round, float roundingPower) const
{
    TRACE;
    auto const win = m_bar->m_window.lock();
    auto const mon = g_pHyprOpenGL->m_renderData.pMonitor.lock();

    // Calculate the bar rect in render space.
    auto const bar_rect = m_bar->getAssignedBoxInGlobalSpace()
                              .translate(-mon->m_position)
                              .translate(win->m_floatingOffset)
                              .expand(1.0)
                              .scale(mon->m_scale)
                              .round();

    auto const bar_and_window_rect =
        [&]() {
            auto box = m_bar->getAssignedBoxInGlobalSpace();
            box.height += win->getWindowMainSurfaceBox().height;
            return box;
        }()
            .translate(-mon->m_position)
            .translate(win->m_floatingOffset)
            .expand(1.0)
            .scale(mon->m_scale)
            .round();

    // Render bar background.
    g_pHyprOpenGL->renderRect(
        bar_and_window_rect,
        g_plugin->config().bar.fill_color(),
        {.round = round, .roundingPower = roundingPower});

    // Render bar text.
    auto const& barcfg = g_plugin->config().bar;
    if (barcfg.text_enabled()) {
        auto const text_texture = g_pHyprOpenGL->renderText(
            win->m_title,
            barcfg.text_color(),
            barcfg.text_size_pts());
        auto text_rect = CBox{bar_rect.pos(), text_texture->m_size}
                             .translate(bar_rect.size() / 2.0)
                             .translate(text_texture->m_size / -2.0)
                             .round();
        text_rect.x = std::max(
            text_rect.x,
            bar_rect.x + g_plugin->config().buttons.padding());
        g_pHyprOpenGL->scissor(
            text_rect.x,
            text_rect.y,
            text_rect.width / 2.0,
            text_rect.height);
        g_pHyprOpenGL->renderTexture(text_texture, text_rect, {});
        g_pHyprOpenGL->scissor(nullptr);
    }
}

void RenderPass::renderButton(
    ButtonInstance const& button,
    Vector2D const& origin,
    float scale_factor) const
{
    TRACE;
    CHyprColor const color = button.model().colorFor(button.state());
    auto const box = button.box().scale(scale_factor).translate(origin).round();
    int const round = button.model().size().x * 0.5 * scale_factor;
    g_pHyprOpenGL->renderRect(box, color, {.round = round});
}

// IPassElement Overrides

void RenderPass::draw(CRegion const&)
{
    TRACE;
    PHLMONITOR const mon = g_pHyprOpenGL->m_renderData.pMonitor.lock();
    auto const win = m_bar->m_window;

    auto const scale_factor = mon->m_scale;
    auto const rounding = win->rounding() * scale_factor;

    // Enable and clear stencil buffer to 0
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xff);
    glClear(GL_STENCIL_BUFFER_BIT);

    // Mask out the window contents using the stencil buffer.
    glStencilFunc(GL_ALWAYS, 1, 0xff);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    auto window_rect = win->getWindowMainSurfaceBox()
                           .translate(-mon->m_position)
                           .translate(win->m_floatingOffset)
                           .scale(mon->m_scale);

    // Shrink the window rect to avoid gaps between the bar and window content
    float const ROUNDING_COMPENSATION = 2.f * scale_factor;
    if (window_rect.width > 2 * ROUNDING_COMPENSATION) {
        window_rect.x += ROUNDING_COMPENSATION;
        window_rect.width =
            std::max(1.0, window_rect.width - 2 * ROUNDING_COMPENSATION);
    }
    if (window_rect.height > 2 * ROUNDING_COMPENSATION) {
        window_rect.y += ROUNDING_COMPENSATION;
        window_rect.height =
            std::max(1.0, window_rect.height - 2 * ROUNDING_COMPENSATION);
    }

    g_pHyprOpenGL->renderRect(
        window_rect.round(),
        CHyprColor{},
        {.round = static_cast<int>(rounding),
         .roundingPower = win->roundingPower()});

    // Now the bar won't draw behind the window contents.
    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilMask(0x00);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Calculate the bar rect in render space.
    auto const bar_topleft = m_bar->getAssignedBoxInGlobalSpace()
                                 .translate(-mon->m_position)
                                 .translate(win->m_floatingOffset)
                                 .scale(mon->m_scale)
                                 .round()
                                 .pos();

    // Draw the bar.
    renderBar(rounding, win->roundingPower());

    // Draw the buttons.
    for (auto const& btn : m_bar->m_btnmgr.buttons()) {
        renderButton(btn, bar_topleft, scale_factor);
    }

    glStencilMask(0xff);
    glClear(GL_STENCIL_BUFFER_BIT);
    glDisable(GL_STENCIL_TEST);
}

bool RenderPass::needsLiveBlur()
{
    return false;
}

bool RenderPass::needsPrecomputeBlur()
{
    return false;
}

char const *RenderPass::passName()
{
    return "deco::RenderPass";
}
