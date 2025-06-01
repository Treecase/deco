#include "config.hpp"

#include <hyprland/src/helpers/Color.hpp>
#include <hyprlang.hpp>
#include <string_view>

#include "log.hpp"
#include "plugin.hpp"

using deco::Plugin;
using deco::config::Config;
using deco::config::Side;
using namespace std::string_view_literals;

// Config /////////////////////////////////////////////////////////////////////

Config::Config(Plugin const& plugin)
: bar{plugin}
, buttons{plugin}
{
    TRACE;
}

// Config::Bar ////////////////////////////////////////////////////////////////

Config::Bar::Bar(Plugin const& plugin)
: m_height{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("bar:height"))}
, m_fillcolor{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("bar:fill_color"))}
{
    TRACE;
}

int Config::Bar::height() const
{
    return **m_height;
}

CHyprColor Config::Bar::fill_color() const
{
    return **m_fillcolor;
}

// Config::Buttons ////////////////////////////////////////////////////////////

Config::Buttons::Buttons(Plugin const& plugin)
: m_side{reinterpret_cast<Hyprlang::STRING const *>(
      plugin.getConfigValue("buttons:side"))}
, m_spacing{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("buttons:spacing"))}
, m_padding{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("buttons:padding"))}
{
    TRACE;
}

Side Config::Buttons::side() const
{
    if ("left"sv == *m_side) {
        return Side::LEFT;
    } else {
        return Side::RIGHT;
    }
}

int Config::Buttons::spacing() const
{
    return **m_spacing;
}

int Config::Buttons::padding() const
{
    return **m_padding;
}
