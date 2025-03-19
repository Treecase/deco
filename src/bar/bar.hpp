#pragma once

#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

namespace deco::bar {

struct EventCallbacks;

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

    UP<EventCallbacks> cbs{};

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
    Factory();

    WP<Bar> createFor(PHLWINDOW window);
    void removeBar(PHLWINDOW window);

private:
    SP<HOOK_CALLBACK_FN> ptr_openWindow;
};

inline Factory g_factory{};

} // namespace deco::bar
