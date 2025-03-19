#pragma once

#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

namespace deco::bar {

class Bar : public IHyprWindowDecoration {
public:
    struct Drag {
        Vector2D start;
    };

    PHLWINDOWREF m_window;
    CBox m_assignedBox{};

    /// WARNING: Internal use only!!!
    explicit Bar(PHLWINDOW);
    virtual ~Bar() = default;

    // IHyprWindowDecoration Overrides
    virtual SDecorationPositioningInfo getPositioningInfo() override;
    virtual void onPositioningReply(
        SDecorationPositioningReply const& reply) override;
    virtual void draw(PHLMONITOR, float const& a) override;
    virtual eDecorationType getDecorationType() override;
    virtual void updateWindow(PHLWINDOW) override;
    virtual void damageEntire() override;
    virtual eDecorationLayer getDecorationLayer() override;
    virtual uint64_t getDecorationFlags() override;
    virtual std::string getDisplayName() override;

    void render() const;

    void startDrag();
    void updateDrag();
    void endDrag();

    // Event Handlers
    bool onMouseButton(IPointer::SButtonEvent);
    bool onMouseMove(Vector2D);

    // Helpers
    CBox getAssignedBoxInGlobalSpace() const;
    CBox getFullRenderArea() const;
    bool isMouseInside() const;

    Vector2D getMouseRelative() const;
    Vector2D getGlobalPointRelative(Vector2D const& point) const;

    double getCenterline() const;

private:
    bool m_was_button_clicked{false};
    bool m_was_clicked{false};
    bool m_is_dragged{false};
    Vector2D m_last_mouse_pos{};
    Vector2D m_last_click_pos{};
};

////////////////////////////////////////////////////////////////////////////////

class Factory {
public:
    WP<Bar> createFor(PHLWINDOW window);

    auto& bars() const { return m_bars; }

private:
    std::vector<WP<Bar>> m_bars{};
};

inline Factory g_factory{};

} // namespace deco::bar
