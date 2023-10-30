/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::builder
{

/**
 * @brief Defines the interface class for the slide view builder.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <slideView vAlign="top" hAlign="left" width="50%" height="15" opacity="1.0" >
        <styleSheet>color: blue; background-color: yellow</styleSheet>
    </slideView>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b hAlign (optional, left/right, default=left): horizontal alignment of the widget.
 * - \b vAlign (optional, top/bottom, default=top): vertical alignment of the widget.
 * - \b hOffset (optional, int, default=0): horizontal offset of the widget (px or %).
 * - \b vOffset (optional, int, default=0): vertical offset of the widget (px or %).
 * - \b width (optional, int, default=100%): width of the widget (px or %).
 * - \b height (optional, int, default=100%): height of the widget (px or %).
 * - \b opacity (optional, float, default=1.0) : widget opacity.
 * - \b animatable (optional, bool, default=false): defines if the slide bar must use animation or not.
 * - \b animatableAlignment (optional, , default=): defines the animation direction.
 * - \b styleSheet (optional, string, default=""): style of the widget (see Qt style sheets
 * http://doc.qt.io/qt-5/stylesheet-examples.html).
 */
class UI_CLASS_API slideview : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(slideview, ui::object);

    UI_API const static std::string REGISTRY_KEY;

    /// Defines the horizontal alignment of the widget.
    enum h_alignment
    {
        right,
        left
    };

    /// Defines the vertical alignment of the widget.
    enum v_alignment
    {
        top,
        bottom
    };

    /// Defines animation direction.
    enum animatable_alignment
    {
        right_animation,
        left_animation,
        top_animation,
        bottom_animation
    };

    /// Creates the slide view builder.
    UI_API slideview() = default;

    /// Destroyes the builder.
    UI_API ~slideview() override = default;

    /// Returns the generic container.
    UI_API virtual ui::container::widget::sptr get_container() const;

    /// Parses the configuration.
    UI_API virtual void initialize(const ui::config_t& _config);

    /// Creates the container.
    UI_API virtual void create_container(ui::container::widget::sptr _parent) = 0;

    /// Destroys the container.
    UI_API virtual void destroy_container() = 0;

protected:

    /// Contains the generic container of the slide view.
    ui::container::widget::sptr m_container;

    /// Defines the horizontal alignment.
    h_alignment m_h_alignment {left};

    /// Defines the vertical alignment.
    v_alignment m_v_alignment {top};

    /// Defines the width of the widget.
    int m_width {100};

    /// Defines if the width must be read a percent.
    bool m_percent_width {true};

    /// Defines the height of the widget.
    int m_height {100};

    /// Defines if the height must be read a percent.
    bool m_percent_height {true};

    /// Defines the horizontal offset of the widget.
    int m_h_offset {0};

    /// Defines if the horizontal offset must be read a percent.
    bool m_percent_h_offset {false};

    /// Defines the vertical offset of the widget.
    int m_v_offset {0};

    /// Defines if the vertical offset must be read a percent.
    bool m_percent_v_offset {false};

    /// Defines the widget opacity.
    double m_opacity {1.};

    /// Defines if the widget animation is enabled.
    bool m_animatable {false};

    /// Defines the animation direction.
    animatable_alignment m_animatable_alignment {top_animation};

    /// Defines the additional style sheet of the widget.
    std::string m_style_sheet;
};

} // namespace sight::ui::builder
