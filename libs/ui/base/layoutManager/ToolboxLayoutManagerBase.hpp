/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
class UI_BASE_CLASS_API ToolboxLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreClassMacro(ToolboxLayoutManagerBase, ui::base::layoutManager::IViewLayoutManager)

    typedef std::string RegistryKeyType;

    class ViewInfo
    {
    public:

        ViewInfo() :
            m_border(0),
            m_minSize(std::make_pair(-1, -1)),
            m_caption(""),
            m_visible(true),
            m_expanded(false),
            m_useScrollBar(false)
        {
        }

        int m_border;
        int m_leftBorder {0};
        int m_topBorder {0};
        int m_rightBorder {0};
        int m_bottomBorder {0};
        std::pair< int, int >  m_minSize;
        std::string m_caption;
        bool m_visible;
        bool m_expanded;
        bool m_useScrollBar;
        /// Background color. Use `default` to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    UI_BASE_API ToolboxLayoutManagerBase();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~ToolboxLayoutManagerBase();

    /**
     * @brief Initialize Line layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="::sight::modules::ui::base::view::SDefaultView" autoConnect="no" >
           <gui>
               <layout type="::ui::base::ToolboxLayoutManager">
                   <view caption="My view 1" expanded="true" />
                   <view caption="My view 2" expanded="true" />
                   <view caption="My view 3" expanded="false" />
                   <spacer />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="no" />
               <view wid="view3WID" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="::ui::base::ToolboxLayoutManager" \> : define a toolbox layout.
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
     *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
     *   - \b expanded  {true | yes | false | no} : define if the view is expanded or not.
     *   - \b useScrollBar  {true | yes | false | no} : define if the view use scrollbar or not.
     */
    UI_BASE_API virtual void initialize( ConfigurationType configuration) override;

    UI_BASE_API static const RegistryKeyType REGISTRY_KEY;

protected:

    //------------------------------------------------------------------------------

    std::list< ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;
};

} // namespace layoutManager
} // namespace sight::ui::base
