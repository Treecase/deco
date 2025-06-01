#include "widgets/button.hpp"

#include <hyprutils/math/Vector2D.hpp>

#include "models/button.hpp"

using deco::ButtonInstance;
using deco::ButtonModel;
using deco::State;

ButtonInstance::ButtonInstance(
    ButtonModel const& model,
    Vector2D const& position)
: m_model{model}
, m_position{position}
{
}

CBox ButtonInstance::box() const
{
    return m_model.box().translate(m_position);
}

ButtonModel const& ButtonInstance::model() const
{
    return m_model;
}

Vector2D ButtonInstance::position() const
{
    return m_position;
}

State ButtonInstance::state() const
{
    return m_state;
}

void ButtonInstance::setState(State const& state)
{
    m_state = state;
}

void ButtonInstance::setPosition(Vector2D const& position)
{
    m_position = position;
}
