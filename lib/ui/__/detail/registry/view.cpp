/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

ui::container::widget::sptr view::get_parent()
{
    ui::container::widget::sptr parent_container;
    if(!m_parent_wid.empty())
    {
        parent_container = ui::registry::get_wid_container(m_parent_wid);
        SIGHT_ASSERT(
            "The parent view of the wid container '" + m_parent_wid + "' is not found. Check that the view is "
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

void view::set_parent(std::string _wid)
{
    SIGHT_ASSERT(
        "The method 'setParent()' is available only if this service declares its parent container with a 'wid'.",
        !m_parent_wid.empty()
    );
    m_parent_wid = std::move(_wid);
}

//-----------------------------------------------------------------------------

void view::initialize(const ui::config_t& _configuration)
{
    m_parent_wid = _configuration.get<std::string>("parent.<xmlattr>.wid", "");

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
            m_sids[sid] = index;
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

    // find menubar
    {
        const std::string sid = _configuration.get<std::string>("menubar.<xmlattr>.sid", "");
        if(!sid.empty())
        {
            const bool start = core::runtime::get_ptree_value(_configuration, "menubar.<xmlattr>.start", false);
            m_menu_bar_sid = std::make_pair(sid, start);
        }
    }

    // find toolbar
    {
        const std::string sid = _configuration.get<std::string>("toolbar.<xmlattr>.sid", "");
        if(!sid.empty())
        {
            const bool start = core::runtime::get_ptree_value(_configuration, "toolbar.<xmlattr>.start", false);
            m_tool_bar_sid = std::make_pair(sid, start);
        }
    }
}

//-----------------------------------------------------------------------------

void view::manage(std::vector<ui::container::widget::sptr> _sub_views)
{
    ui::container::widget::sptr container;
    for(const auto& sid : m_sids)
    {
        SIGHT_ASSERT(
            "The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
            << (sid.second + 1) << " views in <registry>, but only " << _sub_views.size() << " in <layout>.",
            sid.second < _sub_views.size()
        );
        container = _sub_views.at(sid.second);
        ui::registry::register_sid_container(sid.first, container);
    }

    for(const auto& wid : m_wids)
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

void view::manage_menu_bar(ui::container::menubar::sptr _menu_bar)
{
    ui::registry::register_sid_menu_bar(m_menu_bar_sid.first, std::move(_menu_bar));
}

//-----------------------------------------------------------------------------

void view::manage_tool_bar(ui::container::toolbar::sptr _tool_bar)
{
    ui::registry::register_sid_tool_bar(m_tool_bar_sid.first, std::move(_tool_bar));
}

//-----------------------------------------------------------------------------

void view::unmanage()
{
    for(const auto& sid : m_sids)
    {
        ui::registry::unregister_sid_container(sid.first);
    }

    for(const auto& wid : m_wids)
    {
        ui::registry::unregister_wid_container(wid.first);
    }
}

//-----------------------------------------------------------------------------

void view::unmanage_tool_bar()
{
    if(!m_tool_bar_sid.first.empty())
    {
        ui::registry::unregister_sid_tool_bar(m_tool_bar_sid.first);
    }
}

//-----------------------------------------------------------------------------

void view::unmanage_menu_bar()
{
    if(!m_menu_bar_sid.first.empty())
    {
        ui::registry::unregister_sid_menu_bar(m_menu_bar_sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
