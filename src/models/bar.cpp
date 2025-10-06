#include "models/bar.hpp"

#include <any>
#include <functional>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/macros.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <hyprutils/string/VarList.hpp>

#include "config.hpp"
#include "log.hpp"
#include "models/button.hpp"
#include "plugin.hpp"

using deco::BarModel;
using deco::ButtonModel;

// Config Keyword "addButton"
//
// Format:
// `addButton = normal_color, hovered_color, clicked_color, action`
Hyprlang::CParseResult deco::addButton(char const *, char const *args)
{
    TRACE;
    deco::log("Triggered keyword \"addButton\"");
    Hyprlang::CParseResult result{};

    // TODO: Empty strings should be allowed and use the default value.
    // Non-empty invalid strings should cause a parse error.
    Hyprutils::String::CVarList vars{args};
    auto it = vars.begin();
    auto const diameter = g_plugin->configValue<config::buttons::diameter>();
    if (it == vars.end()) {
        result.setError(
            "missing args: normal_color, hovered_color, clicked_color, action");
        return result;
    }
    auto const normal = configStringToInt(*it++);
    if (it == vars.end()) {
        result.setError("missing args: hovered_color, clicked_color, action");
        return result;
    }
    auto const hovered = configStringToInt(*it++);
    if (it == vars.end()) {
        result.setError("missing args: clicked_color, action");
        return result;
    }
    auto const clicked = configStringToInt(*it++);
    if (it == vars.end()) {
        result.setError("missing arg: action");
        return result;
    }
    auto const action = *it;

    ButtonModel const button{
        action,
        diameter,
        static_cast<uint64_t>(normal.value_or(0xff45475a)),
        static_cast<uint64_t>(hovered.value_or(0xff585b70)),
        static_cast<uint64_t>(clicked.value_or(0xff313244))};

    deco::log("New button: {}", button);

    deco::g_plugin->barModel().m_buttons.push_back(button);
    return Hyprlang::CParseResult{};
}

// BarModel ///////////////////////////////////////////////////////////////////

BarModel::BarModel(deco::Plugin& plugin)
: m_plugin{plugin}
{
    TRACE;
    ptr_preConfigReload = m_plugin.addCallback(
        "preConfigReload",
        std::bind_front(&BarModel::onPreConfigReload, this));
    m_plugin.addConfigKeyword("addButton", addButton);
}

int BarModel::height() const
{
    return m_plugin.configValue<config::bar::height>();
}

CHyprColor BarModel::fill() const
{
    return m_plugin.configValue<config::bar::fill_color>();
}

std::vector<ButtonModel> const& BarModel::buttons() const
{
    return m_buttons;
}

// Events /////////////////////////////////////////////////////////////////////
void BarModel::onPreConfigReload(void *, SCallbackInfo const&, std::any const&)
{
    TRACE;
    deco::log("Config reloading");
    m_buttons.clear();
}
