/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
class SIGHT_UI_CLASS_API toolbox : public view
{
public:

    SIGHT_DECLARE_CLASS(toolbox, ui::layout::view);

    using registry_key_t = std::string;

    class view_info
    {
    public:

        int m_border {0};
        int m_left_border {0};
        int m_top_border {0};
        int m_right_border {0};
        int m_bottom_border {0};
        std::pair<int, int> m_min_size {-1, -1};
        std::pair<int, int> m_max_size {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
        std::string m_caption;
        bool m_visible {true};
        bool m_expanded {false};
        bool m_use_scroll_bar {false};

        /// Background color. Use `default` to use the default background color, else, set an hexadecimal value.
        std::string m_background_color;
        /// Defines a qss key to apply style
        std::string m_qss_key;
    };

    /// Constructor. Do nothing.
    SIGHT_UI_API toolbox();

    /// Destructor. Do nothing.
    SIGHT_UI_API ~toolbox() override;

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="sight::module::ui::view" auto_connect="false" >
           <gui>
               <layout type="ui::layout::toolbox">
                    <icon_color value="#FFFFFF" />
                   <view caption="My view 1" expanded="true" />
                   <view caption="My view 2" expanded="true" />
                   <view caption="My view 3" expanded="false" />
                   <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" />
               <view sid="view2"  />
               <view wid="view3WID" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::layout::toolbox" /\> : define a toolbox layout.
     *  - \<icon_color  value="#FFFFFF" /\> : determines the color fo the triangle-like icon that expands sub-views.
     *  - \<view caption="My view 1" expanded="true" minWidth="30" /\> : define a new view which can have the following
     * attributes
     *   - \b border : defines the size of the border in the layout
     *   - \b leftBorder : defines the size of the left border in the layout
     *   - \b topBorder : defines the size of the top border in the layout
     *   - \b rightBorder : defines the size of the right border in the layout
     *   - \b bottomBorder : defines the size of the bottom border in the layout
     *   - \b caption : define the title to display.
     *   - \b minWidth  : minimal width of the view.
     *   - \b minHeight : minimal height of the view.
     *   - \b visible  {true | false} : define if the view is visible or not.
     *   - \b expanded  {true |false } : define if the view is expanded or not.
     *   - \b useScrollBar  {true | false} : define if the view use scrollbar or not.
     */
    SIGHT_UI_API void initialize(const ui::config_t& _configuration) override;

    SIGHT_UI_API static const registry_key_t REGISTRY_KEY;

protected:

    //------------------------------------------------------------------------------

    std::list<view_info> get_views_info()
    {
        return m_views;
    }

    //------------------------------------------------------------------------------

    std::string get_icon_color()
    {
        return m_icon_color;
    }

private:

    /// Save layout configuration definition
    std::list<view_info> m_views;

    /// Determines the color fo the triangle-like icon that expands sub-views.
    std::string m_icon_color;
};

} // namespace sight::ui::layout
