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
#include <map>

namespace sight::ui::base
{
namespace layoutManager
{

/**
 * @brief   Defines the base for cardinal layout manager.
 */
class UI_BASE_CLASS_API TabLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreClassMacro(TabLayoutManagerBase, ui::base::layoutManager::IViewLayoutManager)

    typedef std::string RegistryKeyType;

    class ViewInfo
    {
    public:

        ViewInfo() :
            m_border(0),
            m_minSize(std::make_pair(-1, -1)),
            m_caption(""),
            m_isSelect(false),
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
        bool m_isSelect;
        bool m_useScrollBar;
        /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    UI_BASE_API TabLayoutManagerBase();

    /// Destructor. Do nothing.
    UI_BASE_API virtual ~TabLayoutManagerBase();

    /**
     * @brief Initialize tab layout manager before the creation of layout.
     *
     * Example of configuration with Tab layout.
     * @code{.xml}
       <service uid="subView3" type="::modules::ui::base::view::SDefaultView" autoConnect="no" >
           <gui>
               <layout type="::ui::base::TabLayoutManager" >
                   <view caption="TabView1" />
                   <view caption="TabView2" selected="yes" border="10" />
                   <view caption="TabView3"  />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
               <view sid="view3" start="yes" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="::ui::base::TabLayoutManager" \> : define a tab layout.
     *  - \<view caption="TabView1" selected="center" /\> : define a new view which can have the following attributes
     *   - \b border : defines the size of the border in the layout
     *   - \b leftBorder : defines the size of the left border in the layout
     *   - \b topBorder : defines the size of the top border in the layout
     *   - \b rightBorder : defines the size of the right border in the layout
     *   - \b bottomBorder : defines the size of the bottom border in the layout
     *   - \b caption : define the title to display.
     *   - \b minWidth  : minimal width of the view.
     *   - \b minHeight : minimal height of the view.
     *   - \b selected  {yes | no}: define if the tab is the current one.
     */
    UI_BASE_API virtual void initialize( ConfigurationType configuration) override;

    UI_BASE_API static const RegistryKeyType REGISTRY_KEY;

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
