/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "view.hpp"

#include "ui/__/container/menubar.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/registry.hpp"

#include <core/runtime/helper.hpp>

#include <service/op.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

view::view(std::string _sid) :
    m_sid(std::move(_sid))
{
}

//-----------------------------------------------------------------------------

view::~view()
= default;

//-----------------------------------------------------------------------------

ui::container::widget::sptr view::getParent()
{
    ui::container::widget::sptr parent_container;
    if(!m_parentWid.empty())
    {
        parent_container = ui::registry::get_wid_container(m_parentWid);
        SIGHT_ASSERT(
            "The parent view of the wid container '" + m_parentWid + "' is not found. Check that the view is "
                                                                     "properly declared and the service is started.",
            parent_container
        );
    }
    else
    {
        parent_container = ui::registry::get_sid_container(m_sid);
        SIGHT_ASSERT(
            "The parent view of the service '" + m_sid + "' is not found. Check that the view is "
                                                         "properly declared and the service is started.",
            parent_container
        );
    }

    return parent_container;
}

//-----------------------------------------------------------------------------

void view::setParent(std::string _wid)
{
    SIGHT_ASSERT(
        "The method 'setParent()' is available only if this service declares its parent container with a 'wid'.",
        !m_parentWid.empty()
    );
    m_parentWid = std::move(_wid);
}

//-----------------------------------------------------------------------------

void view::initialize(const ui::config_t& _configuration)
{
    m_parentWid = _configuration.get<std::string>("parent.<xmlattr>.wid", "");

    // index represents associated container with position in subviews vector
    unsigned int index = 0;

    // initialize m_sids and m_wids map with configuration
    const auto views_it       = _configuration.equal_range("view");
    const auto slide_views_it = _configuration.equal_range("slideView");

    // Extracts a ptree from an iterator
    auto it_to_tree_fn = [](auto& _it)
                         {
                             return _it.second;
                         };

    // Gather views and slideviews ptrees to process them in a single loop
    std::vector<boost::property_tree::ptree> views_vector;
    std::transform(views_it.first, views_it.second, std::back_inserter(views_vector), it_to_tree_fn);
    std::transform(slide_views_it.first, slide_views_it.second, std::back_inserter(views_vector), it_to_tree_fn);

    for(const auto& view : views_vector)
    {
        const std::string sid = view.get<std::string>("<xmlattr>.sid", "");
        const std::string wid = view.get<std::string>("<xmlattr>.wid", "");

        SIGHT_ASSERT("<view> tag must have sid or wid attribute", !sid.empty() || !wid.empty());
        if(!sid.empty())
        {
            const bool start = core::runtime::get_ptree_value(view, "<xmlattr>.start", false);
            m_sids[sid] = SIDContainerMapType::mapped_type(index, start);
        }
        else if(!wid.empty())
        {
            m_wids[wid] = index;
        }
        else
        {
            SIGHT_FATAL("Unreachable code");
        }

        index++;
    }

    // find menuBar
    {
        const std::string sid = _configuration.get<std::string>("menuBar.<xmlattr>.sid", "");
        if(!sid.empty())
        {
            const bool start = core::runtime::get_ptree_value(_configuration, "menuBar.<xmlattr>.start", false);
            m_menuBarSid = std::make_pair(sid, start);
        }
    }

    // find toolBar
    {
        const std::string sid = _configuration.get<std::string>("toolBar.<xmlattr>.sid", "");
        if(!sid.empty())
        {
            const bool start = core::runtime::get_ptree_value(_configuration, "toolBar.<xmlattr>.start", false);
            m_toolBarSid = std::make_pair(sid, start);
        }
    }
}

//-----------------------------------------------------------------------------

void view::manage(std::vector<ui::container::widget::sptr> _sub_views)
{
    ui::container::widget::sptr container;
    for(const SIDContainerMapType::value_type& sid : m_sids)
    {
        SIGHT_ASSERT(
            "The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
            << (sid.second.first + 1) << " views in <registry>, but only " << _sub_views.size() << " in <layout>.",
            sid.second.first < _sub_views.size()
        );
        container = _sub_views.at(sid.second.first);
        ui::registry::register_sid_container(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The service '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' view, "
                                                                                                 "the service may be created later if it uses deferred objects, thus use start=\"no\" and start "
                                                                                                 "it at the end of the configuration",
                core::tools::id::exist(
                    sid.first
                )
            );
            service::base::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->stopped()
            );
            service->start();
        }
    }

    for(const WIDContainerMapType::value_type& wid : m_wids)
    {
        SIGHT_ASSERT(
            "The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
            << (wid.second + 1) << " views in <registry>, but only " << _sub_views.size() << " in <layout>.",
            wid.second < _sub_views.size()
        );
        container = _sub_views.at(wid.second);
        ui::registry::register_wid_container(wid.first, container);
    }
}

//-----------------------------------------------------------------------------

void view::manageMenuBar(ui::container::menubar::sptr _menu_bar)
{
    ui::registry::register_sid_menu_bar(m_menuBarSid.first, std::move(_menu_bar));
    if(m_menuBarSid.second) //service is auto started?
    {
        SIGHT_ASSERT(
            "The menuBar service '" + m_menuBarSid.first + "' declared by '" + m_sid + "' does not exist.",
            core::tools::id::exist(m_menuBarSid.first)
        );
        service::base::sptr service = service::get(m_menuBarSid.first);
        service->start();
    }
}

//-----------------------------------------------------------------------------

void view::manageToolBar(ui::container::toolbar::sptr _tool_bar)
{
    ui::registry::register_sid_tool_bar(m_toolBarSid.first, std::move(_tool_bar));
    if(m_toolBarSid.second) //service is auto started?
    {
        SIGHT_ASSERT(
            "The toolBar service '" + m_toolBarSid.first + "' declared by '" + m_sid + "' does not exist.",
            core::tools::id::exist(m_toolBarSid.first)
        );
        service::base::sptr service = service::get(m_toolBarSid.first);
        service->start();
    }
}

//-----------------------------------------------------------------------------

void view::unmanage()
{
    for(const SIDContainerMapType::value_type& sid : m_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The view '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                   "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_container(sid.first);
    }

    for(const WIDContainerMapType::value_type& wid : m_wids)
    {
        ui::registry::unregister_wid_container(wid.first);
    }
}

//-----------------------------------------------------------------------------

void view::unmanageToolBar()
{
    if(!m_toolBarSid.first.empty())
    {
        if(m_toolBarSid.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The toolBar service '" + m_toolBarSid.first + "' declared by '" + m_sid + "' does not exist.",
                core::tools::id::exist(m_toolBarSid.first)
            );
            service::base::sptr service = service::get(m_toolBarSid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_tool_bar(m_toolBarSid.first);
    }
}

//-----------------------------------------------------------------------------

void view::unmanageMenuBar()
{
    if(!m_menuBarSid.first.empty())
    {
        if(m_menuBarSid.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menuBar service '" + m_menuBarSid.first + "' declared by '" + m_sid + "' does not exist.",
                core::tools::id::exist(m_menuBarSid.first)
            );
            service::base::sptr service = service::get(m_menuBarSid.first);
            service->stop().wait();
        }

        ui::registry::unregister_sid_menu_bar(m_menuBarSid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
