#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>
#include <type_traits>

#include "include.hpp"

#ifdef DECO_VALUEMACRO
#  define VALUE(_prefix, _name, _type, _default)                     \
      namespace _prefix {                                            \
      struct _name {                                                 \
          using TYPE = _type;                                        \
          static constexpr char const *NAME =                        \
              "plugin:deco:" #_prefix ":" #_name;                    \
          static constexpr TYPE DEFAULT = _default;                  \
          static TYPE get()                                          \
          {                                                          \
              auto const value =                                     \
                  deco::config::get<deco::config::_prefix::_name>(); \
              return value;                                          \
          }                                                          \
      };                                                             \
      }
#endif

namespace deco::config {

template<class V>
concept is_hyprlang_string =
    std::is_same<typename V::TYPE, Hyprlang::STRING>::value;

/// Get a config value.
template<class Value>
inline static Value::TYPE get()
    requires(!is_hyprlang_string<Value>)
{
    auto const cptr = HyprlandAPI::getConfigValue(deco::g_handle, Value::NAME);
    if (cptr == nullptr) {
        throw std::runtime_error{
            std::format("getConfigValue error for '{}'", Value::NAME)};
    }
    return **reinterpret_cast<Value::TYPE *const *>(cptr->getDataStaticPtr());
}

/// Get a config value.
template<class Value>
inline static Hyprlang::STRING get()
    requires(is_hyprlang_string<Value>)
{
    auto const cptr = HyprlandAPI::getConfigValue(deco::g_handle, Value::NAME);
    if (cptr == nullptr) {
        throw std::runtime_error{
            std::format("getConfigValue error for '{}'", Value::NAME)};
    }
    return *reinterpret_cast<Hyprlang::STRING const *>(
        cptr->getDataStaticPtr());
}

} // namespace deco::config
