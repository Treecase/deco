#include "models/bar.hpp"

#include <any>
#include <functional>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <hyprutils/string/VarList.hpp>
#include <src/macros.hpp>
#include <stdexcept>

#include "include.hpp"
#include "log.hpp"
#include "models/button.hpp"
#include "plugin.hpp"

using deco::BarModel;
using deco::ButtonModel;

// Config Keyword "addButton"
//
// Format:
// `addButton = diameter, normal_color, hovered_color, clicked_color, action`
Hyprlang::CParseResult deco::addButton(char const *, char const *args)
{
    TRACE;
    deco::log("Triggered keyword \"addButton\"");

    // TODO: Empty strings should be allowed and use the default value.
    // Non-empty invalid strings should cause a parse error.
    Hyprutils::String::CVarList vars{args};
    auto it = vars.begin();
    if (it == vars.end()) {
        throw std::runtime_error{"invalid addButton args"};
    }
    auto const diameter = g_plugin->config().buttons.diameter();
    if (it == vars.end()) {
        throw std::runtime_error{"invalid addButton args"};
    }
    auto const normal = configStringToInt(*it++);
    if (it == vars.end()) {
        throw std::runtime_error{"invalid addButton args"};
    }
    auto const hovered = configStringToInt(*it++);
    if (it == vars.end()) {
        throw std::runtime_error{"invalid addButton args"};
    }
    auto const clicked = configStringToInt(*it++);
    if (it == vars.end()) {
        throw std::runtime_error{"invalid addButton args"};
    }
    auto const action = *it;

    ButtonModel const button{
        action,
        diameter,
        static_cast<uint64_t>(normal.value_or(0xff45475a)),
        static_cast<uint64_t>(hovered.value_or(0xff585b70)),
        static_cast<uint64_t>(clicked.value_or(0xff313244))};

    deco::log("New button: {}", button);

    RASSERT(g_plugin, "plugin not initialized");
    if (!g_plugin) {
        throw std::runtime_error{"plugin not initialized!"};
    }
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
    return m_plugin.config().bar.height();
}

CHyprColor BarModel::fill() const
{
    return m_plugin.config().bar.fill_color();
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
