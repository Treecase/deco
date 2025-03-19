#include "config/config.hpp"

/// Register a config value with Hyprland.
template<class Value>
static void registerValue()
{
    deco::log("Register config value '{}'", Value::NAME);
    HyprlandAPI::addConfigValue(deco::g_handle, Value::NAME, Value::DEFAULT);
}

///////////////////////////////////////////////////////////////////////////////

void deco::config::init_values()
{
    deco::log("Register config values");
    registerValue<config::bar::height>();
    registerValue<config::bar::fill_color>();
    registerValue<config::buttons::side>();
    registerValue<config::buttons::padding>();
    registerValue<config::buttons::spacing>();
}
