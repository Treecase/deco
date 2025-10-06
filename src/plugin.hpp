#pragma once

#include <any>
#include <cstdint>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/memory/Memory.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprlang.hpp>
#include <string>

#include "config.hpp"
#include "models/bar.hpp"

namespace deco {

class Plugin {
public:
    explicit Plugin(HANDLE);
    void init();

    BarModel& barModel();
    config::Config const& config();

    void notify(
        std::string const& text,
        eIcons icon,
        uint64_t time = 5000,
        CHyprColor const& color = CHyprColor{0}) const;

    [[nodiscard]]
    SP<HOOK_CALLBACK_FN> addCallback(
        std::string const& event,
        HOOK_CALLBACK_FN callback);

    void addConfigKeyword(
        std::string const& keyword,
        Hyprlang::PCONFIGHANDLERFUNC handler);

    void addConfigValue(
        std::string const& name,
        Hyprlang::CConfigValue const& value);

    void *const *getConfigValue(std::string const& name) const;

private:
    HANDLE const m_handle;
    UP<config::Config> m_config{nullptr};
    UP<BarModel> m_barModel{nullptr};

    SP<HOOK_CALLBACK_FN> ptr_openWindow{nullptr};
    void onOpenWindow(void *, SCallbackInfo&, std::any);

    SDispatchResult command_decobars(std::string const&);
};

extern UP<Plugin> g_plugin;

} // namespace deco
