#pragma once

#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprlang.hpp>

namespace deco {
class Plugin;

namespace config {

    enum class Side { LEFT, RIGHT };

    class Config {
    public:
        class Bar {
        public:
            Bar(Plugin const&);
            int height() const;
            CHyprColor fill_color() const;
            bool text_enabled() const;
            CHyprColor text_color() const;
            int text_size_pts() const;

        private:
            Hyprlang::INT *const *const m_height;
            Hyprlang::INT *const *const m_fillcolor;
            Hyprlang::INT *const *const m_textenable;
            Hyprlang::INT *const *const m_textcolor;
            Hyprlang::INT *const *const m_textsize;
        };

        class Buttons {
        public:
            Buttons(Plugin const&);
            Side side() const;
            int spacing() const;
            int padding() const;

        private:
            Hyprlang::STRING const *const m_side;
            Hyprlang::INT *const *const m_spacing;
            Hyprlang::INT *const *const m_padding;
        };

        Config(Plugin const&);

        Bar const bar;
        Buttons const buttons;
    };

} // namespace config
} // namespace deco
