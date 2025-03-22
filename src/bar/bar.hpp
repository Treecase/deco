#pragma once
#include <expected>
#include <format>
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

    void hide(bool hide = true);

    bool isHidden() const { return m_is_hidden; }

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
    bool m_is_hidden{false};
    bool m_was_button_clicked{false};
    bool m_was_clicked{false};
    bool m_is_dragged{false};
    Vector2D m_last_mouse_pos{};
    Vector2D m_last_click_pos{};
};

////////////////////////////////////////////////////////////////////////////////

class Factory {
public:
    enum class CreateForError { DoesntWantDecorations, Failed };

    Factory();

    std::expected<WP<Bar>, CreateForError> createFor(PHLWINDOW window);
    void removeBar(PHLWINDOW window);

private:
    SP<HOOK_CALLBACK_FN> ptr_openWindow;
};

inline Factory g_factory{};

} // namespace deco::bar

template<>
struct std::formatter<deco::bar::Factory::CreateForError> {
    template<class FmtContext>
    FmtContext::iterator format(
        deco::bar::Factory::CreateForError const& err,
        FmtContext& ctx) const
    {
        switch (err) {
        case deco::bar::Factory::CreateForError::Failed:
            return std::ranges::copy(
                       "HyprlandAPI::addWindowDecoration failed",
                       ctx.out())
                .out;
        case deco::bar::Factory::CreateForError::DoesntWantDecorations:
            return std::ranges::copy(
                       "Window doesn't want decorations",
                       ctx.out())
                .out;
        }
        return ctx.out();
    }

    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error(
                "Invalid format args for Factory::CreateForError.");
        } else {
            return it;
        }
    }
};
