#include "config/config.hpp"

/// Register a config value with Hyprland.
template<class Value>
static void registerValue()
{
    HyprlandAPI::addConfigValue(deco::g_handle, Value::NAME, Value::DEFAULT);
    deco::log("Registered config value '{}'", Value::NAME);
}

///////////////////////////////////////////////////////////////////////////////

void deco::config::init_values()
{
    registerValue<config::bar::height>();
    registerValue<config::bar::fill_color>();
    registerValue<config::buttons::side>();
    registerValue<config::buttons::padding>();
    registerValue<config::buttons::spacing>();
    deco::log("Registered config values");
}
