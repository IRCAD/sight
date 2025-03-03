/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/ui/__/config.hpp>

#include "ui/__/layout/view.hpp"

#include <core/base.hpp>

#include <list>

namespace sight::ui::layout
{

/**
 * @brief   Defines the base for line layout manager.
 */
class SIGHT_UI_CLASS_API line : public view
{
public:

    SIGHT_DECLARE_CLASS(line, ui::layout::view);

    /// Defines all possible orientation for a LineLayout
    enum orientation
    {
        vertical,
        horizontal
    };

    using registry_key_t = std::string;

    class view_info
    {
    public:

        int m_proportion {1};
        int m_border {0};
        int m_left_border {0};
        int m_top_border {0};
        int m_right_border {0};
        int m_bottom_border {0};
        int m_spacing {-1};
        std::pair<int, int> m_min_size {-1, -1};
        std::pair<int, int> m_max_size {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
        std::pair<bool, std::string> m_caption {false, ""};
        bool m_visible {true};
        bool m_is_spacer {false};
        bool m_use_scroll_bar {false};
        std::string m_tool_tip;

        /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
        std::string m_background_color;
        /// Defines a qss key to apply style
        std::string m_qss_key;
    };

    /// Constructor. Do nothing.
    SIGHT_UI_API line() = default;

    /// Destructor. Do nothing.
    SIGHT_UI_API ~line() override = default;

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="sight::module::ui::view" auto_connect="false" >
           <gui>
              <layout type="ui::layout::line" >
                    <orientation value="horizontal" />
                    <view proportion="0" minWidth="30" />
                    <view proportion="0" minWidth="50" />
                    <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="true" />
               <view sid="view2" start="true" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::layout::line" \> : define a line layout.
     *  - \<spacer /> : create a stretch view.
     *  - \<view proportion="0" minWidth="30" /\> : define a new view which can have the following attributes
     *   - \b orientation {horizontal | vertical} : define if the layout is vertical or horizontal.
     *   - \b proportion : define if the space allocate in proportion of the proportion factor (bigger it is, more space
     * are used).
     *   - \b border : defines the size of the border in the layout
     *   - \b leftBorder : defines the size of the left border in the layout
     *   - \b topBorder : defines the size of the top border in the layout
     *   - \b rightBorder : defines the size of the right border in the layout
     *   - \b bottomBorder : defines the size of the bottom border in the layout
     *   - \b spacing: space between widgets in the layout, if it's -1, nothing will be sets end the default value will
     * be used.
     *   - \b caption : define the title to display.
     *   - \b minWidth  : minimal width of the view.
     *   - \b minHeight : minimal height of the view.
     *   - \b visible  {true | false} : define if the view is visible or not.
     *   - \b toolTip : string that will be displayed next to the mouse pointer when it lies over the view.
     *   - \b backgroundColor (optional) : (hexa) background color.
     */

    SIGHT_UI_API void initialize(const ui::config_t& _configuration) override;

    SIGHT_UI_API static const registry_key_t REGISTRY_KEY;

    //------------------------------------------------------------------------------

    orientation get_orientation()
    {
        return m_orientation;
    }

    //------------------------------------------------------------------------------

    int get_spacing()
    {
        return m_spacing;
    }

protected:

    //------------------------------------------------------------------------------

    std::list<view_info> get_views_info()
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::list<view_info> m_views;

    orientation m_orientation {vertical};
    int m_spacing {-1};
};

} // namespace sight::ui::layout
