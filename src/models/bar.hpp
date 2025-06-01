#pragma once

#include <any>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <vector>

#include "models/button.hpp"

namespace deco {

class Plugin;

Hyprlang::CParseResult addButton(char const *, char const *);

class BarModel {
public:
    BarModel(Plugin&);

    int height() const;
    CHyprColor fill() const;
    std::vector<ButtonModel> const& buttons() const;

private:
    Plugin& m_plugin;
    std::vector<ButtonModel> m_buttons{};

    // Events
    void onPreConfigReload(void *, SCallbackInfo const&, std::any const&);
    SP<HOOK_CALLBACK_FN> ptr_preConfigReload{nullptr};

    // Keyword
    friend Hyprlang::CParseResult deco::addButton(char const *, char const *);
};

} // namespace deco
