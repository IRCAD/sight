/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

namespace sight::ui::base::layoutManager
{

/**
 * @brief   Defines the base for cardinal layout manager.
 */
class UI_BASE_CLASS_API CardinalLayoutManagerBase : public IViewLayoutManager
{
public:

    SIGHT_DECLARE_CLASS(CardinalLayoutManagerBase, ui::base::layoutManager::IViewLayoutManager);

    /// Defines all possible positions for a CardinalLayout
    typedef enum
    {
        CENTER,
        RIGHT,
        LEFT,
        BOTTOM,
        TOP
    } Align;

    typedef std::string RegistryKeyType;

    class ViewInfo
    {
    public:

        Align m_align {CENTER};
        std::pair<int, int> m_minSize {-1, -1};
        std::pair<int, int> m_maxSize {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
        bool m_visible {true};
        bool m_isResizable {true};
        int m_position {0};
        int m_layer {0};
        int m_row {0};
        std::pair<bool, std::string> m_caption {false, ""};
        bool m_useScrollBar {false};
        std::string m_toolTip;

        /// Background color. When given an empty string, a default background is used. To use another color, set an
        /// hexadecimal value (it has to start with "#").
        std::string m_backgroundColor;
    };

    UI_BASE_API CardinalLayoutManagerBase()           = default;
    UI_BASE_API ~CardinalLayoutManagerBase() override = default;

    /**
     * @brief Initializes cardinal layout manager. Must be called before the layout creation.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="sight::module::ui::base::SView" autoConnect="false" >
           <gui>
               <layout type="ui::base::CardinalLayoutManager" >
                   <view caption="CardinalView1" align="center" />
                   <view caption="CardinalView2" align="right" minWidth="400" />
                   <view caption="CardinalView3" align="bottom" minHeight="400" />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="true" />
               <view sid="view2" start="true" />
               <view sid="view3" start="true" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="ui::base::CardinalLayoutManager" \> : define a cardinal layout.
     *  - \<view caption="CardinalView1" align="center" /\> : define a new view with following attribute
     *   - \b caption : name of the view (display on the screen).
     *   - \b align  {center | bottom | top | right | left}: define the position of the view
     *   - \b minWidth : minimal width of the view
     *   - \b minHeight : minimal height of the view
     *   - \b resizable  {true | false}: define if the view can be resized.
     *   - \b position : indicates the sequential position, starting with zero. It uses if more than one view as the
     * same align value (available only with wxWidget see wxAuiManager in wxWidgets documentation for more details).
     *   - \b layer : available only with wxWidget. See wxAuiManager in wxWidgets documentation for more details
     *   - \b row : use to place several view next to each other (available only with wxWidget). See wxAuiManager in
     * wxWidgets documentation for more details
     *   - \b visible  {true | false} : define if the view is visible or not.
     *   - \b toolTip : string that will be displayed next to the mouse pointer when it lies over the view.
     *   - \b backgroundColor (optional) : (hexadecimal format starting with "#") background color.
     */

    UI_BASE_API void initialize(const ui::base::config_t& configuration) override;

    UI_BASE_API static const RegistryKeyType REGISTRY_KEY;

protected:

    //------------------------------------------------------------------------------

    std::list<ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    static const std::map<std::string, Align> STRING_TO_ALIGN;

    /// Saves layout configuration definition
    std::list<ViewInfo> m_views;
};

} // namespace sight::ui::base::layoutManager
