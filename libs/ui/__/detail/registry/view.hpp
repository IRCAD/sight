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
class UI_CLASS_API view : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(view, ui::object);

    //------------------------------------------------------------------------------

    static UI_API sptr make(const std::string& _sid)
    {
        return std::make_shared<view>(_sid);
    }

    /// Constructor.
    UI_API view(std::string _sid);

    /// Destructor. Do nothing
    UI_API ~view() override;

    UI_API virtual SPTR(ui::container::widget) get_parent();

    UI_API virtual void set_parent(std::string _wid);

    /**
     * @brief Initialize managers.
     *
     * Example of configuration
     * @code{.xml}
        <service uid="subview1" type="sight::module::ui::view" auto_connect="false" >
            <gui>
                <layout type="ui::layout::line" >
                    <orientation value="horizontal" />
                    <view caption="view3" />
                    <view caption="view4" />
                    <view caption="view5" />
                </layout>
                <toolbar />
                <menubar />
            </gui>
            <registry>
                <parent wid="my_view" />
                <toolbar sid="toolbar" start="true" />
                <menubar sid="menubar" start="true" />
                <view sid="subview3" start="true" />
                <view wid="subview4" />
                <view wid="subview5" />
            </registry>
        </service>
       @endcode
     * - \<parent wid="my_view" /\> : This section isn't mandatory. When the parent section is present, the wid
     * attribute
     * is used to find its parent container.
     * Otherwise, the service uid is used to find its parent container.
     * - \<toolbar sid="toolbar1" start="true" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the tool bar identifier.
     *   - \b start = {true| false} (default value false): indicate if the tool bar service must be started by the
     * service.
     * - \<menubar sid="menubar1" start="true" /\> : This section isn't mandatory.
     *   - \b sid  (mandatory): the menu bar identifier.
     *   - \b start = {true| false} (default value false): indicate if the menu bar service must be started by the
     * service.
     * - \<view sid="subview3" start="true" /\> : define the service of the view to add in the container.
     *   - \b sid  (mandatory): the service identifier.
     *   - \b start = {true| false} (default value false): indicate if the view service must be started by the service.
     * - \<view wid="subview4" /\> : reserve a sub container for the view in the parent container with the name
     *"subview4". The service which want to use this sub container
     *   will have define a parent with \<parent wid="subview4" /\>.
     *   - \b wid  (mandatory): the window identifier.
     *
     * @warning
     * - The number of view item in the gui section must be equal or greater than in the registry section.
     * - The order of the view in each section (gui and registry) must be the same.\n
     *   For example: the view caption "view3" will be connected with the service which have the sid = "subview3" (It
     * could also be a wid).
     *
     * @throw core::exception if the configuration element does not start with <registry>
     *
     */
    UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Starting view manager.
     * All services managed in local subviews
     * and with start="true" in configuration will be started.
     * @pre view must be initialized before.
     * @pre sub containers must be instanced before.
     */
    UI_API virtual void manage(std::vector<SPTR(ui::container::widget)> _sub_views);

    /**
     * @brief Register menu bar.
     * If start="true" in configuration the menu bar services will be started.
     */
    UI_API virtual void manage_menu_bar(SPTR(ui::container::menubar) _menu_bar);

    /**
     * @brief Register tool bar.
     * If start="true" in configuration the tool bar services will be started.
     */
    UI_API virtual void manage_tool_bar(SPTR(ui::container::toolbar) _tool_bar);

    /**
     * @brief Stopping view manager.
     * All services managed in local subviews will be stopped.
     */
    UI_API virtual void unmanage();

    /**
     * @brief Stopping view manager.
     * tool_bar service will be stopped.
     */
    UI_API virtual void unmanage_tool_bar();

    /**
     * @brief Stopping view manager.
     * menu_bar service will be stopped.
     */
    UI_API virtual void unmanage_menu_bar();

protected:

    using sid_container_map_type = std::map<std::string, std::pair<unsigned int, bool> >;
    using wid_container_map_type = std::map<std::string, unsigned int>;
    using sid_menu_bar_pair_type = std::pair<std::string, bool>;
    using sid_tool_bar_pair_type = std::pair<std::string, bool>;

    /**
     * @brief All services ID managed and associated with pair containing:
     * subviews index vector and boolean describing if is started by the manager.
     */
    sid_container_map_type m_sids;

    /// All windows containers ID managed (and associated with subviews index vector).
    wid_container_map_type m_wids;

    /// menu bar service ID associate with this view
    sid_menu_bar_pair_type m_menu_bar_sid;

    /// Tool bar service ID associate with this view
    sid_tool_bar_pair_type m_tool_bar_sid;

    std::string m_parent_wid;
    SPTR(ui::container::widget) m_parent_container;

    /// Main service ID associate with this view
    std::string m_sid;
};

} // namespace detail::registry

} // namespace sight::ui
