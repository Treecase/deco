#pragma once

#include <hyprland/src/render/pass/PassElement.hpp>

#include "bar.hpp"

namespace deco::bar {

class RenderPass : public IPassElement {
public:
    RenderPass(Bar *bar);

    // IPassElement Overrides
    virtual void draw(CRegion const& damage) override;
    virtual bool needsLiveBlur() override;
    virtual bool needsPrecomputeBlur() override;
    virtual char const *passName() override;

private:
    Bar *m_bar;
};

} // namespace deco::bar
