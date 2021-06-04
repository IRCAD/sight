/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/layoutManager/IViewLayoutManager.hpp"

#include <core/base.hpp>

#include <list>

namespace sight::ui::base
{

namespace layoutManager
{

/**
 * @brief   Defines the base for line layout manager.
 */
class UI_BASE_CLASS_API LineLayoutManagerBase : public IViewLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(LineLayoutManagerBase, ui::base::layoutManager::IViewLayoutManager);

    /// Defines all possible orientation for a LineLayout
    typedef enum
    {
        VERTICAL,
        HORIZONTAL
    } Orientation;

    typedef std::string RegistryKeyType;

    class ViewInfo
    {
    public:

        ViewInfo() :
            m_proportion(1),
            m_border(0),
            m_minSize(std::make_pair(-1, -1)),
            m_caption(std::make_pair(false, "")),
            m_visible(true),
            m_isSpacer(false),
            m_useScrollBar(false)
        {
        }

        int m_proportion;
        int m_border;
        int m_leftBorder {0};
        int m_topBorder {0};
        int m_rightBorder {0};
        int m_bottomBorder {0};
        int m_spacing {-1};
        std::pair<int, int> m_minSize;
        std::pair<bool, std::string> m_caption;
        bool m_visible;
        bool m_isSpacer;
        bool m_useScrollBar;
        std::string m_toolTip;

        /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    UI_BASE_API LineLayoutManagerBase();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~LineLayoutManagerBase();

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="sight::module::ui::base::SView" autoConnect="false" >
           <gui>
              <layout type="ui::base::LineLayoutManager" >
                    <orientation value="horizontal" />
                    <view proportion="0" minWidth="30" />
                    <view proportion="0" minWidth="50" />
                    <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::base::LineLayoutManager" \> : define a line layout.
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
     *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
     *   - \b toolTip : string that will be displayed next to the mouse pointer when it lies over the view.
     *   - \b backgroundColor (optional) : (hexa) background color.
     */

    UI_BASE_API void initialize(ConfigurationType configuration) override;

    UI_BASE_API static const RegistryKeyType REGISTRY_KEY;

    //------------------------------------------------------------------------------

    Orientation getOrientation()
    {
        return m_orientation;
    }

protected:

    //------------------------------------------------------------------------------

    std::list<ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::list<ViewInfo> m_views;

    Orientation m_orientation;
};

} // namespace layoutManager

} // namespace sight::ui::base
