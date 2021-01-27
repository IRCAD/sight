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

#include "gui/config.hpp"
#include "gui/container/fwContainer.hpp"
#include "gui/container/fwMenuBar.hpp"
#include "gui/container/fwToolBar.hpp"
#include "gui/GuiBaseObject.hpp"

#include <core/runtime/ConfigurationElement.hpp>

namespace sight::gui
{
namespace container
{
class fwContainer;
class fwMenuBar;
class fwToolBar;
}

namespace registrar
{

/**
 * @brief   Defines the generic view manager for IHM.
 *
 */
class GUI_CLASS_API ViewRegistrar : public gui::GuiBaseObject
{

public:

    fwCoreClassMacro(ViewRegistrar, gui::GuiBaseObject)

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<ViewRegistrar>(sid);
    }

    /// Constructor.
    GUI_API ViewRegistrar( const std::string& sid);

    /// Destructor. Do nothing
    GUI_API virtual ~ViewRegistrar();

    GUI_API virtual SPTR(gui::container::fwContainer) getParent();

    GUI_API virtual void setParent(std::string wid);

    /**
     * @brief Initialize managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="subView1" type="::modules::gui::view::SDefaultView" autoConnect="no" >
            <gui>
                <layout type="::gui::LineLayoutManager" >
                    <orientation value="horizontal" />
                    <view caption="view3" />
                    <view caption="view4" />
                    <view caption="view5" />
                </layout>
                <toolBar />
                <menuBar />
            </gui>
            <registry>
                <parent wid="myView" />
                <toolBar sid="toolbar" start="yes" />
                <menuBar sid="menubar" start="yes" />
                <view sid="subView3" start="yes" />
                <view wid="subView4" />
                <view wid="subView5" />
            </registry>
        </service>
       @endcode
     * - \<parent wid="myView" /\> : This section isn't mandatory. When the parent section is present, the wid attribute
     * is used to find its parent container.
     * Otherwise, the service uid is used to find its parent container.
     * - \<toolBar sid="toolbar1" start="yes" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the tool bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the tool bar service must be started by the service.
     * - \<menuBar sid="menubar1" start="yes" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the menu bar identifier.
     *   - \b start = {yes| no} (default value no): indicate if the menu bar service must be started by the service.
     * - \<view sid="subView3" start="yes" /\> : define the service of the view to add in the container.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {yes| no} (default value no): indicate if the view service must be started by the service.
     * - \<view wid="subView4" /\> : reserve a sub container for the view in the parent container with the name
     *"subView4". The service which want to use this sub container
     *   will have define a parent with \<parent wid="subView4" /\>.
     *   - \b wid  (mandatory): the window identifier.
     *
     * @warning
     * - The number of view item in the gui section must be equal or greater than in the registry section.
     * - The order of the view in each section (gui and registry) must be the same.\n
     *   For example: the view caption "view3" will be connected with the service which have the sid = "subView3" (It
     * could also be a wid).
     *
     */
    GUI_API virtual void initialize( core::runtime::ConfigurationElement::sptr configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subViews
     * and with start="yes" in configuration will be started.
     * @pre ViewRegistrar must be initialized before.
     * @pre sub containers must be instanced before.
     */
    GUI_API virtual void manage(std::vector< SPTR(gui::container::fwContainer) > subViews );

    /**
     * @brief Register menu bar.
     * If start="yes" in configuration the menu bar services will be started.
     */
    GUI_API virtual void manageMenuBar(SPTR(gui::container::fwMenuBar) menuBar );

    /**
     * @brief Register tool bar.
     * If start="yes" in configuration the tool bar services will be started.
     */
    GUI_API virtual void manageToolBar(SPTR(gui::container::fwToolBar) toolBar );

    /**
     * @brief Stopping view manager.
     * All services managed in local subViews will be stopped.
     */
    GUI_API virtual void unmanage();

    /**
     * @brief Stopping view manager.
     * ToolBar service will be stopped.
     */
    GUI_API virtual void unmanageToolBar();

    /**
     * @brief Stopping view manager.
     * MenuBar service will be stopped.
     */
    GUI_API virtual void unmanageMenuBar();

protected:

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;
    typedef std::map< std::string, std::pair<unsigned int, bool> > SIDContainerMapType;
    typedef std::map< std::string, unsigned int > WIDContainerMapType;
    typedef std::pair< std::string, bool > SIDMenuBarPairType;
    typedef std::pair< std::string, bool > SIDToolBarPairType;

    /**
     * @brief All services ID managed and associated with pair containing:
     * subViews index vector and boolean describing if is started by the manager.
     */
    SIDContainerMapType m_sids;

    /// All windows containers ID managed (and associated with subViews index vector).
    WIDContainerMapType m_wids;

    /// Menu bar service ID associate with this ViewRegistrar
    SIDMenuBarPairType m_menuBarSid;

    /// Tool bar service ID associate with this ViewRegistrar
    SIDToolBarPairType m_toolBarSid;

    std::string m_parentWid;
    SPTR(gui::container::fwContainer) m_parentContainer;

    /// Main service ID associate with this ViewRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace sight::gui
