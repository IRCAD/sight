/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
class FWGUI_CLASS_API TabLayoutManagerBase : public IViewLayoutManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (TabLayoutManagerBase)(::fwGui::layoutManager::IViewLayoutManager) )

    typedef std::string RegistryKeyType;

    class ViewInfo
    {
    public:

        ViewInfo() :
            m_caption(""),
            m_isSelect(false),
            m_useScrollBar(false)
        {
        }

        std::string m_caption;
        bool m_isSelect;
        bool m_useScrollBar;
    };

    /// Constructor. Do nothing.
    FWGUI_API TabLayoutManagerBase();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~TabLayoutManagerBase();

    /**
     * @brief Initialize tab layout manager before the creation of layout.
     *
     * Example of configuration with Tab layout.
     * @code{.xml}
       <service uid="subView3" type="::gui::view::SDefaultView" autoConnect="no" >
           <gui>
               <layout type="::fwGui::TabLayoutManager" >
                   <view caption="TabView1" />
                   <view caption="TabView2" selected="yes" />
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
     *  - \<layout type="::fwGui::TabLayoutManager" \> : define a tab layout.
     *  - \<view caption="TabView1" selected="center" /\> : define a new view which can have the following attributes
     *   - \b caption : name of the view (display on the screen).
     *   - \b selected  {yes | no}: define if the tab is the current one.
     */

    FWGUI_API virtual void initialize( ConfigurationType configuration) override;

    FWGUI_API static const RegistryKeyType REGISTRY_KEY;

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
} // namespace fwGui
