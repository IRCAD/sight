/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/layout/view.hpp"

#include <core/base.hpp>

#include <list>
#include <map>

namespace sight::ui::layout
{

/**
 * @brief   Defines the base for cardinal layout manager.
 */
class UI_CLASS_API tab : public view
{
public:

    SIGHT_DECLARE_CLASS(tab, ui::layout::view);

    typedef std::string registry_key_t;

    class ViewInfo
    {
    public:

        int m_border {0};
        int m_leftBorder {0};
        int m_topBorder {0};
        int m_rightBorder {0};
        int m_bottomBorder {0};
        std::pair<int, int> m_minSize {-1, -1};
        std::string m_caption;
        bool m_isSelect {false};
        bool m_useScrollBar {false};

        /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    UI_API tab() = default;

    /// Destructor. Do nothing.
    UI_API ~tab() override = default;

    /**
     * @brief Initialize tab layout manager before the creation of layout.
     *
     * Example of configuration with Tab layout.
     * @code{.xml}
       <service uid="subView3" type="sight::module::ui::view" auto_connect="false" >
           <gui>
               <layout type="ui::layout::tab" >
                   <view caption="TabView1" />
                   <view caption="TabView2" selected="true" border="10" />
                   <view caption="TabView3"  />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="true" />
               <view sid="view2" start="true" />
               <view sid="view3" start="true" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::layout::tab" \> : define a tab layout.
     *  - \<view caption="TabView1" selected="center" /\> : define a new view which can have the following attributes
     *   - \b border : defines the size of the border in the layout
     *   - \b leftBorder : defines the size of the left border in the layout
     *   - \b topBorder : defines the size of the top border in the layout
     *   - \b rightBorder : defines the size of the right border in the layout
     *   - \b bottomBorder : defines the size of the bottom border in the layout
     *   - \b caption : define the title to display.
     *   - \b minWidth  : minimal width of the view.
     *   - \b minHeight : minimal height of the view.
     *   - \b selected  {true | false}: define if the tab is the current one.
     */
    UI_API void initialize(const ui::config_t& _configuration) override;

    UI_API static const registry_key_t REGISTRY_KEY;

    //------------------------------------------------------------------------------

    std::list<ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::list<ViewInfo> m_views;
};

} // namespace sight::ui::layout
