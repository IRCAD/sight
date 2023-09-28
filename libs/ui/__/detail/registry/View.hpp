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
#include "ui/__/container/menubar.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui
{

namespace container
{

class widget;
class menubar;
class toolbar;

} // namespace container

namespace detail::registry
{

/**
 * @brief   Defines the generic view manager for IHM.
 *
 */
class UI_CLASS_API View : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(View, ui::object);

    //------------------------------------------------------------------------------

    static UI_API sptr make(const std::string& sid)
    {
        return std::make_shared<View>(sid);
    }

    /// Constructor.
    UI_API View(std::string sid);

    /// Destructor. Do nothing
    UI_API ~View() override;

    UI_API virtual SPTR(ui::container::widget) getParent();

    UI_API virtual void setParent(std::string wid);

    /**
     * @brief Initialize managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="subView1" type="sight::module::ui::SView" autoConnect="false" >
            <gui>
                <layout type="ui::layout::line" >
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
                <toolBar sid="toolbar" start="true" />
                <menuBar sid="menubar" start="true" />
                <view sid="subView3" start="true" />
                <view wid="subView4" />
                <view wid="subView5" />
            </registry>
        </service>
       @endcode
     * - \<parent wid="myView" /\> : This section isn't mandatory. When the parent section is present, the wid attribute
     * is used to find its parent container.
     * Otherwise, the service uid is used to find its parent container.
     * - \<toolBar sid="toolbar1" start="true" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the tool bar identifier.
     *   - \b start = {true| false} (default value false): indicate if the tool bar service must be started by the
     * service.
     * - \<menuBar sid="menubar1" start="true" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the menu bar identifier.
     *   - \b start = {true| false} (default value false): indicate if the menu bar service must be started by the
     * service.
     * - \<view sid="subView3" start="true" /\> : define the service of the view to add in the container.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the view service must be started by the service.
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
     * @throw core::exception if the configuration element does not start with <registry>
     *
     */
    UI_API virtual void initialize(const ui::config_t& configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subViews
     * and with start="true" in configuration will be started.
     * @pre View must be initialized before.
     * @pre sub containers must be instanced before.
     */
    UI_API virtual void manage(std::vector<SPTR(ui::container::widget)> subViews);

    /**
     * @brief Register menu bar.
     * If start="true" in configuration the menu bar services will be started.
     */
    UI_API virtual void manageMenuBar(SPTR(ui::container::menubar) menuBar);

    /**
     * @brief Register tool bar.
     * If start="true" in configuration the tool bar services will be started.
     */
    UI_API virtual void manageToolBar(SPTR(ui::container::toolbar) toolBar);

    /**
     * @brief Stopping view manager.
     * All services managed in local subViews will be stopped.
     */
    UI_API virtual void unmanage();

    /**
     * @brief Stopping view manager.
     * ToolBar service will be stopped.
     */
    UI_API virtual void unmanageToolBar();

    /**
     * @brief Stopping view manager.
     * MenuBar service will be stopped.
     */
    UI_API virtual void unmanageMenuBar();

protected:

    typedef std::map<std::string, std::pair<unsigned int, bool> > SIDContainerMapType;
    typedef std::map<std::string, unsigned int> WIDContainerMapType;
    typedef std::pair<std::string, bool> SIDMenuBarPairType;
    typedef std::pair<std::string, bool> SIDToolBarPairType;

    /**
     * @brief All services ID managed and associated with pair containing:
     * subViews index vector and boolean describing if is started by the manager.
     */
    SIDContainerMapType m_sids;

    /// All windows containers ID managed (and associated with subViews index vector).
    WIDContainerMapType m_wids;

    /// Menu bar service ID associate with this View
    SIDMenuBarPairType m_menuBarSid;

    /// Tool bar service ID associate with this View
    SIDToolBarPairType m_toolBarSid;

    std::string m_parentWid;
    SPTR(ui::container::widget) m_parentContainer;

    /// Main service ID associate with this View
    std::string m_sid;
};

} // namespace detail::registry

} // namespace sight::ui
