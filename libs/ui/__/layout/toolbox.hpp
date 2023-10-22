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

namespace sight::ui::layout
{

/**
 * @brief   Defines the base for line layout manager.
 */
class UI_CLASS_API toolbox : public view
{
public:

    SIGHT_DECLARE_CLASS(toolbox, ui::layout::view);

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
        std::pair<int, int> m_maxSize {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
        std::string m_caption;
        bool m_visible {true};
        bool m_expanded {false};
        bool m_useScrollBar {false};

        /// Background color. Use `default` to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    UI_API toolbox();

    /// Destructor. Do nothing.
    UI_API ~toolbox() override;

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="sight::module::ui::view" auto_connect="false" >
           <gui>
               <layout type="ui::layout::toolbox">
                   <view caption="My view 1" expanded="true" />
                   <view caption="My view 2" expanded="true" />
                   <view caption="My view 3" expanded="false" />
                   <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="true" />
               <view sid="view2" start="false" />
               <view wid="view3WID" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::layout::toolbox" \> : define a toolbox layout.
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
    UI_API void initialize(const ui::config_t& _configuration) override;

    UI_API static const registry_key_t REGISTRY_KEY;

protected:

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
