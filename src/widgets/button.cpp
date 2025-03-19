#include "widgets/button.hpp"

using namespace widget;

Button::Button(deco::ButtonModel const& m, Vector2D const& p)
: model{m}
, position{p}
{
}

CBox Button::box() const
{
    return model.box().translate(position);
}

void Button::render(Vector2D const& origin, double scale) const
{
    CHyprColor color;
    switch (state) {
    case CLICKED:
        color = model.clicked;
        break;
    case NORMAL:
        color = model.normal;
        break;
    case HOVERED:
        color = model.hovered;
        break;
    }

    g_pHyprOpenGL->renderRect(
        model.box().translate(position).scale(scale).translate(origin).round(),
        color,
        model.diameter * 0.5 * scale);
}
