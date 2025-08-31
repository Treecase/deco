#pragma once
#include <cstdint>
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprutils/math/Vector2D.hpp>
#include <optional>

#include "bar/buttonmanager.hpp"

namespace deco::bar {

class RenderPass;

class Bar : public IHyprWindowDecoration {
public:
    PHLWINDOWREF m_window;
    CBox m_assignedBox{};

    explicit Bar(PHLWINDOW);
    virtual ~Bar() = default;

    // IHyprWindowDecoration //////////////////////////////////////////////////
    virtual SDecorationPositioningInfo getPositioningInfo() override;
    virtual void onPositioningReply(
        SDecorationPositioningReply const& reply) override;
    virtual void draw(PHLMONITOR, float const& a) override;
    virtual eDecorationType getDecorationType() override;
    virtual void updateWindow(PHLWINDOW) override;
    virtual void damageEntire()
        override; // should be ignored by non-absolute decos
    virtual bool onInputOnDeco(eInputType const, Vector2D const&, std::any = {})
        override;
    virtual eDecorationLayer getDecorationLayer() override;
    virtual uint64_t getDecorationFlags() override;
    virtual std::string getDisplayName() override;
    // IHyprWindowDecoration //////////////////////////////////////////////////

    void hide(bool hide = true);
    bool isHidden() const;
    bool isHiddenManual() const;
    bool isHiddenRule() const;
    bool isVisible() const;
    void regenerateButtons();

    // Helpers
    CBox getAssignedBoxInGlobalSpace() const;
    bool isMouseInside() const;
    bool isEventValid() const;

    Vector2D getMouseRelative() const;
    Vector2D getGlobalPointRelative(Vector2D const& point) const;
    double getCenterline() const;

private:
    struct Drag {
        Vector2D start;
        explicit Drag(Vector2D const&);
        ~Drag();
    };

    friend class deco::bar::RenderPass;

    std::optional<Drag> m_drag{};
    bool m_manual_hide{false};
    bool m_rule_hide{false};
    std::optional<int> m_rule_height{std::nullopt};
    ButtonManager m_btnmgr;

    Vector2D m_mouse_pos{};

    // Event handlers
    SP<HOOK_CALLBACK_FN> ptr_configReloaded{nullptr};
    SP<HOOK_CALLBACK_FN> ptr_mouseButton{nullptr};
    SP<HOOK_CALLBACK_FN> ptr_mouseMove{nullptr};
    SP<HOOK_CALLBACK_FN> ptr_windowUpdateRules{nullptr};

    void onConfigReloaded(void *, SCallbackInfo&, std::any);
    void onMouseButton(void *, SCallbackInfo&, std::any);
    void onMouseMove(void *, SCallbackInfo&, std::any);
    void onWindowUpdateRules(void *, SCallbackInfo&, std::any);
};

} // namespace deco::bar
