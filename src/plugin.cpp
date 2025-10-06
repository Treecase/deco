#include "plugin.hpp"

#include <algorithm>
#include <cstdint>
#include <format>
#include <functional>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/macros.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprutils/memory/UniquePtr.hpp>
#include <stdexcept>
#include <string>

#include "bar/bar.hpp"
#include "config.hpp"
#include "defines.hpp"
#include "log.hpp"
#include "models/bar.hpp"

using deco::BarModel;
using deco::Plugin;
using deco::bar::Bar;

Plugin::Plugin(HANDLE handle)
: m_handle{handle}
{
    TRACE;
    if (m_handle == nullptr) {
        throw std::runtime_error{"[deco] Handle must not be NULL"};
    }

    // ALWAYS add this to your plugins. It will prevent random crashes coming
    // from mismatched header versions.
    std::string const HASH = __hyprland_api_get_hash();
    if (HASH != GIT_COMMIT_HASH) {
        notify("Mismatched headers! Can't proceed.", ICON_ERROR);
        throw std::runtime_error("[deco] Version mismatch");
    }

    deco::log("Initializing config values");

    addConfigValue<config::bar::height>();
    addConfigValue<config::bar::fill_color>();
    addConfigValue<config::bar::text_enabled>();
    addConfigValue<config::bar::text_color>();
    addConfigValue<config::bar::text_size>();
    addConfigValue<config::buttons::side>();
    addConfigValue<config::buttons::spacing>();
    addConfigValue<config::buttons::padding>();
    addConfigValue<config::buttons::diameter>();

    HyprlandAPI::addDispatcherV2(
        m_handle,
        "deco",
        std::bind_front(&Plugin::command_decobars, this));

    deco::log("Registered config values");
}

void Plugin::init()
{
    TRACE;

    m_barModel = makeUnique<BarModel>(*this);

    ptr_openWindow =
        addCallback("openWindow", std::bind_front(&Plugin::onOpenWindow, this));

    deco::log("Adding bars to existing windows");
    SCallbackInfo ci{};
    for (auto window : g_pCompositor->m_windows) {
        onOpenWindow(nullptr, ci, window);
    }
}

BarModel& Plugin::barModel()
{
    return *m_barModel;
}

void Plugin::notify(
    std::string const& text,
    eIcons icon,
    uint64_t time,
    CHyprColor const& color) const
{
    TRACE;
    std::string const msg =
        std::format("[deco {}] {}", DECO_PLUGIN_VERSION, text);
    HyprlandAPI::addNotificationV2(
        m_handle,
        {
            { "text",   msg},
            { "time",  time},
            {"color", color},
            { "icon",  icon}
    });
}

SP<HOOK_CALLBACK_FN> Plugin::addCallback(
    std::string const& event,
    HOOK_CALLBACK_FN callback)
{
    TRACE;
    RASSERT(m_handle, "null handle");
    RASSERT(callback, "null callback");
    deco::log("Adding event callback for \"{}\"", event);
    auto const ptr =
        HyprlandAPI::registerCallbackDynamic(m_handle, event, callback);
    if (!ptr) {
        throw std::runtime_error(
            std::format(
                "[deco] Callback registration for \"{}\" event failed",
                event));
    } else {
        return ptr;
    }
}

void Plugin::addConfigKeyword(
    std::string const& keyword,
    Hyprlang::PCONFIGHANDLERFUNC handler)
{
    TRACE;
    auto const success = HyprlandAPI::addConfigKeyword(
        m_handle,
        keyword,
        handler,
        Hyprlang::SHandlerOptions{});
    if (success) {
        deco::log("Added keyword \"{}\"", keyword);
    } else {
        throw std::runtime_error{
            std::format("Failed to add keyword \"{}\"", keyword)};
    }
}

void Plugin::addConfigValue(
    std::string const& name,
    Hyprlang::CConfigValue const& value)
{
    TRACE;
    auto const key = "plugin:deco:" + name;
    auto const success = HyprlandAPI::addConfigValue(m_handle, key, value);
    if (success) {
        deco::log("Added config value \"{}\"", key);
    } else {
        throw std::runtime_error{
            std::format("Failed to add config value \"{}\"", key)};
    }
}

void *const *Plugin::getConfigValue(std::string const& name) const
{
    TRACE;
    auto const key = "plugin:deco:" + name;
    auto const cfgval = HyprlandAPI::getConfigValue(m_handle, key);
    if (cfgval == nullptr) {
        throw std::runtime_error{
            std::format("Failed to get config value \"{}\"", key)};
    }
    auto const ptr = cfgval->getDataStaticPtr();
    if (ptr == nullptr) {
        throw std::runtime_error{
            std::format("Failed to get static pointer for \"{}\"", key)};
    }
    return ptr;
}

void Plugin::onOpenWindow(void *, SCallbackInfo&, std::any data)
{
    TRACE;
    auto const window = std::any_cast<PHLWINDOW>(data);

    if (window->m_X11DoesntWantBorders) {
        deco::log("New window {} doesn't want decorations", window);
        return;
    }
    deco::log("Creating bar for new window {}", window);

    auto bar = makeUnique<Bar>(window);
    auto const success =
        HyprlandAPI::addWindowDecoration(m_handle, window, std::move(bar));
    if (!success) {
        deco::err("Failed to create bar for {}", window);
    }
}

SDispatchResult Plugin::command_decobars(std::string const& args)
{
    static auto const getActiveWindowBar = []() -> Bar * {
        auto window_it = std::ranges::find_if(
            g_pCompositor->m_windows,
            [](auto& w) { return g_pCompositor->isWindowActive(w); });
        if (window_it == g_pCompositor->m_windows.end()) {
            return nullptr;
        }
        deco::log("Found active window: \"{}\"", window_it->get()->m_title);
        auto bar_it = std::ranges::find_if(
            window_it->get()->m_windowDecorations,
            [](UP<IHyprWindowDecoration>& d) {
                return d->getDisplayName() == "deco::Bar";
            });
        if (bar_it == window_it->get()->m_windowDecorations.end()) {
            return nullptr;
        }
        deco::log("Found active bar");
        return dynamic_cast<Bar *>(bar_it->get());
    };

    TRACE;

    if (args == "on") {
        auto bar = getActiveWindowBar();
        if (bar) {
            deco::log("Bar visibility on");
            bar->hide(false);
        } else {
            return SDispatchResult{
                .passEvent = false,
                .success = false,
                .error = "Failed to find active window/bar"};
        }
    } else if (args == "off") {
        auto bar = getActiveWindowBar();
        if (bar) {
            deco::log("Bar visibility off");
            bar->hide(true);
        } else {
            return SDispatchResult{
                .passEvent = false,
                .success = false,
                .error = "Failed to find active window/bar"};
        }
    } else if (args == "toggle") {
        auto bar = getActiveWindowBar();
        if (bar) {
            deco::log("Bar visibility toggled");
            bar->hide(!bar->isHiddenManual());
        } else {
            return SDispatchResult{
                .passEvent = false,
                .success = false,
                .error = "Failed to find active window/bar"};
        }
    } else {
        return SDispatchResult{
            .passEvent = false,
            .success = false,
            .error = "Invalid argument.\nUsage: deco on|off|toggle"};
    }
    return SDispatchResult{.passEvent = false, .success = true, .error = "Ok"};
}
