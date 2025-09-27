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
, m_textenable{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("bar:text_enabled"))}
, m_textcolor{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("bar:text_color"))}
, m_textsize{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("bar:text_size"))}
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

bool Config::Bar::text_enabled() const
{
    return **m_textenable;
}

CHyprColor Config::Bar::text_color() const
{
    return **m_textcolor;
}

int Config::Bar::text_size_pts() const
{
    return std::max(Hyprlang::INT{0}, **m_textsize);
}

// Config::Buttons ////////////////////////////////////////////////////////////

Config::Buttons::Buttons(Plugin const& plugin)
: m_side{reinterpret_cast<Hyprlang::STRING const *>(
      plugin.getConfigValue("buttons:side"))}
, m_spacing{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("buttons:spacing"))}
, m_padding{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("buttons:padding"))}
, m_diameter{reinterpret_cast<Hyprlang::INT *const *>(
      plugin.getConfigValue("buttons:diameter"))}
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

int Config::Buttons::diameter() const
{
    return **m_diameter;
}
