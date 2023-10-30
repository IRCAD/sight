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

#include "ui/__/layout/toolbar_manager.hpp"

#include "ui/__/toolbar.hpp"

#include <core/runtime/path.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::layout
{

const toolbar_manager::registry_key_t toolbar_manager::REGISTRY_KEY = "sight::ui::layout::toolbar";

//-----------------------------------------------------------------------------

std::vector<toolbar_manager::action_info> configure(
    const std::pair<std::string,
                    boost::property_tree::ptree>& _tool_bar_item
)
{
    std::vector<toolbar_manager::action_info> infos;
    if(_tool_bar_item.first == "menuItem")
    {
        toolbar_manager::action_info info;

        info.m_name     = _tool_bar_item.second.get<std::string>("<xmlattr>.name");
        info.m_shortcut = _tool_bar_item.second.get<std::string>("<xmlattr>.shortcut", info.m_shortcut);

        const auto icon = _tool_bar_item.second.get<std::string>("<xmlattr>.icon", "");
        if(!icon.empty())
        {
            info.m_icon = core::runtime::get_module_resource_file_path(icon);
        }

        const auto icon2 = _tool_bar_item.second.get<std::string>("<xmlattr>.icon2", "");
        if(!icon2.empty())
        {
            SIGHT_ASSERT("'icon' attribute must be defined before 'icon2'", !info.m_icon.empty());
            info.m_icon2 = core::runtime::get_module_resource_file_path(icon2);
        }

        if(const auto style = _tool_bar_item.second.get_optional<std::string>("<xmlattr>.style"); style.has_value())
        {
            info.m_is_checkable = (*style == "check");
            info.m_is_radio     = (*style == "radio");
        }

        infos.push_back(info);
    }
    else if(_tool_bar_item.first == "separator")
    {
        toolbar_manager::action_info info;
        info.m_is_separator = true;
        info.m_size         = _tool_bar_item.second.get<int>("<xmlattr>.size", info.m_size);
        infos.push_back(info);
    }
    else if(_tool_bar_item.first == "spacer")
    {
        toolbar_manager::action_info info;
        info.m_is_spacer = true;
        infos.push_back(info);
    }
    else if(_tool_bar_item.first == "menu")
    {
        toolbar_manager::action_info info;
        info.m_is_menu = true;
        info.m_name    = _tool_bar_item.second.get<std::string>("<xmlattr>.name", "");

        const auto icon = _tool_bar_item.second.get<std::string>("<xmlattr>.icon", "");
        if(!icon.empty())
        {
            info.m_icon = core::runtime::get_module_resource_file_path(icon);
        }

        infos.push_back(info);
    }
    else if(_tool_bar_item.first == "editor")
    {
        toolbar_manager::action_info info;
        info.m_is_editor = true;
        infos.push_back(info);
    }
    else if(_tool_bar_item.first == "accordion")
    {
        for(bool first = true ; const auto& [tag, subtree] : _tool_bar_item.second)
        {
            SIGHT_ASSERT("accordions of accordions aren't supported", tag != "accordion");
            SIGHT_ASSERT("separators in accordions aren't supported", tag != "separator");
            toolbar_manager::action_info info = configure({tag, subtree})[0];
            if(first)
            {
                SIGHT_ASSERT(
                    "the first element in an accordion list must be a menuItem with style=\"check\"",
                    info.m_is_checkable
                );
            }

            info.m_accordion = first ? toolbar_manager::accordion::first : toolbar_manager::accordion::yes;
            infos.push_back(info);
            first = false;
        }
    }

    return infos;
}

//------------------------------------------------------------------------------

void toolbar_manager::initialize(const ui::config_t& _configuration)
{
    if(const auto style = _configuration.get_optional<std::string>("<xmlattr>.style"); style.has_value())
    {
        if(*style == "ToolButtonIconOnly" || *style == "ToolButtonTextOnly" || *style == "ToolButtonTextBesideIcon"
           || *style == "ToolButtonTextUnderIcon" || *style == "ToolButtonFollowStyle")
        {
            m_style = *style;
        }
        else
        {
            SIGHT_ERROR(
                "`Style` attribute value must be `ToolButtonIconOnly` "
                "or `ToolButtonTextOnly` or `ToolButtonTextBesideIcon` "
                "or `ToolButtonTextUnderIcon` or `ToolButtonFollowStyle`"
            );
        }
    }

    m_unify_button_size = _configuration.get<bool>("<xmlattr>.uniformSize", m_unify_button_size);

    for(const auto& tool_bar_item : _configuration)
    {
        std::vector<action_info> new_action_infos = configure(tool_bar_item);
        std::ranges::move(new_action_infos, std::back_inserter(m_action_info));
    }
}

//-----------------------------------------------------------------------------

void toolbar_manager::destroy_actions()
{
    for(const ui::container::menu_item::sptr& menu_item : m_menu_items)
    {
        menu_item->destroy_container();
    }

    m_menu_items.clear();
    for(const ui::container::menu::sptr& menu : m_menus)
    {
        menu->destroy_container();
    }

    m_menus.clear();
    for(const ui::container::widget::sptr& container : m_containers)
    {
        container->destroy_container();
    }

    m_containers.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu_item::sptr> toolbar_manager::get_menu_items()
{
    return this->m_menu_items;
}

//-----------------------------------------------------------------------------

std::vector<ui::container::menu::sptr> toolbar_manager::get_menus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

std::vector<ui::container::widget::sptr> toolbar_manager::get_containers()
{
    return this->m_containers;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
