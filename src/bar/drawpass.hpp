#pragma once

#include <hyprland/src/render/pass/PassElement.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/math/Vector2D.hpp>

#include "bar.hpp"
#include "widgets/button.hpp"

namespace deco::bar {

class RenderPass : public IPassElement {
public:
    RenderPass(Bar const *);

    // IPassElement Overrides
    virtual void draw(CRegion const&) override;
    virtual bool needsLiveBlur() override;
    virtual bool needsPrecomputeBlur() override;
    virtual char const *passName() override;

private:
    Bar const *m_bar;

    void renderBar(CBox const&, int, float) const;
    void renderButton(deco::ButtonInstance const&, Vector2D const&, float)
        const;
};

} // namespace deco::bar
