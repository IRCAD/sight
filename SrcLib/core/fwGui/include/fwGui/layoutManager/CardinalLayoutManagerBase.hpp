/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGui/config.hpp"
#include "fwGui/layoutManager/IViewLayoutManager.hpp"

#include <fwCore/base.hpp>

#include <list>
#include <map>

namespace fwGui
{

namespace layoutManager
{

/**
 * @brief   Defines the base for cardinal layout manager.
 */
class FWGUI_CLASS_API CardinalLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreClassMacro(CardinalLayoutManagerBase, ::fwGui::layoutManager::IViewLayoutManager)

    /// Defines all possible position for a CardinalLayout
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

        ViewInfo() :
            m_align(CENTER),
            m_minSize(std::make_pair(-1, -1)),
            m_visible(true),
            m_isResizable(true),
            m_position(0),
            m_layer(0),
            m_row(0),
            m_caption(std::make_pair(false, "")),
            m_useScrollBar(false)
        {
        }

        Align m_align;
        std::pair< int, int >            m_minSize;
        bool m_visible;
        bool m_isResizable;
        int m_position;
        int m_layer;
        int m_row;
        std::pair< bool, std::string >   m_caption;
        bool m_useScrollBar;
        std::string m_toolTip;
        /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
        std::string m_backgroundColor;
    };

    /// Constructor. Do nothing.
    FWGUI_API CardinalLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~CardinalLayoutManagerBase();

    /**
     * @brief Initialize cardinal layout manager before the creation of layout.
     *
     * Example of configuration with cardinal layout.
     * @code{.xml}
       <service uid="subView2" type="::gui::view::SDefaultView" autoConnect="no" >
           <gui>
               <layout type="::fwGui::CardinalLayoutManager" >
                   <view caption="CardinalView1" align="center" />
                   <view caption="CardinalView2" align="right" minWidth="400" />
                   <view caption="CardinalView3" align="bottom" minHeight="400" />
               </layout>
           </gui>
           <registry>
               <view sid="view1" start="yes" />
               <view sid="view2" start="yes" />
               <view sid="view3" start="yes" />
           </registry>
       </service>
       @endcode
     *  - \<layout type="::fwGui::CardinalLayoutManager" \> : define a cardinal layout.
     *  - \<view caption="CardinalView1" align="center" /\> : define a new view with following attribute
     *   - \b caption : name of the view (display on the screen).
     *   - \b align  {center | bottom | top | right | left}: define the position of the view
     *   - \b minWidth : minimal width of the view
     *   - \b minHeight : minimal height of the view
     *   - \b resizable  {yes | no}: define if the view can be resized.
     *   - \b position : indicates the sequential position, starting with zero. It uses if more than one view as the
     * same align value (available only with wxWidget see wxAuiManager in wxWidgets documenattion for more details).
     *   - \b layer : available only with wxWidget. See wxAuiManager in wxWidgets documenattion for more details
     *   - \b row : use to place several view next to each other (available only with wxWidget). See wxAuiManager in
     * wxWidgets documenattion for more details
     *   - \b visible  {true | yes | false | no} : define if the view is visible or not.
     *   - \b toolTip : string that will be displayed next to the mouse pointer when it lies over the view.
     *   - \b backgroundColor (optional) : (hexa) background color.
     */

    FWGUI_API virtual void initialize( ConfigurationType configuration) override;

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

protected:

    //------------------------------------------------------------------------------

    std::list< ViewInfo> getViewsInfo()
    {
        return m_views;
    }

private:

    static const std::map<std::string, Align> STRING_TO_ALIGN;

    /// Save layout configuration definition
    std::list< ViewInfo>  m_views;

};

} // namespace layoutManager
} // namespace fwGui
