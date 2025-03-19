#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>
#include <hyprutils/string/VarList.hpp>

#include "config/config.hpp"
#include "models/bar.hpp"
#include "models/button.hpp"

using Hyprutils::String::CVarList;

static Hyprlang::CParseResult kw_addButton(char const *k, char const *v);

///////////////////////////////////////////////////////////////////////////////

void deco::config::init_keywords()
{
    deco::log("Register config keywords");
    HyprlandAPI::addConfigKeyword(
        deco::g_handle,
        "addButton",
        kw_addButton,
        Hyprlang::SHandlerOptions{});
}

//// KEYWORD HANDLERS /////////////////////////////////////////////////////////

static Hyprlang::CParseResult kw_addButton(char const *k, char const *v)
{
    deco::log("keyword addButton");
    CVarList vars{v};
    auto it = vars.begin();
    auto x = it->empty();

    auto const diameter = configStringToInt(*it++);
    auto const normal = configStringToInt(*it++);
    auto const hovered = configStringToInt(*it++);
    auto const clicked = configStringToInt(*it++);
    auto const action = *it;

    deco::ButtonModel button{};
    if (diameter) {
        button.diameter = *diameter;
    }
    if (normal) {
        button.normal = *normal;
    }
    if (hovered) {
        button.hovered = *hovered;
    }
    if (clicked) {
        button.clicked = *clicked;
    }
    button.action = action;

    deco::log("New button: {}", button);

    deco::g_barmodel.buttons.push_back(button);
    return Hyprlang::CParseResult{};
}
