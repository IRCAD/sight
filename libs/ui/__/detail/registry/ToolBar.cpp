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

#include "ToolBar.hpp"

#include "ui/__/action.hpp"
#include "ui/__/registry.hpp"

#include <service/op.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::detail::registry
{

//-----------------------------------------------------------------------------

ToolBar::ToolBar(std::string sid) :
    m_sid(std::move(sid))
{
}

//-----------------------------------------------------------------------------

ui::container::toolbar::sptr ToolBar::getParent()
{
    return ui::registry::getSIDToolBar(m_sid);
}

//-----------------------------------------------------------------------------

ui::container::menu_item::sptr ToolBar::getFwMenuItem(
    std::string actionSid,
    std::vector<ui::container::menu_item::sptr> menuItems
)
{
    SIGHT_ASSERT(
        "The action '" + actionSid + "' declared by the toolBar '" + m_sid + "' is not found",
        m_actionSids.find(actionSid) != m_actionSids.end()
    );
    ui::container::menu_item::sptr menuItem = menuItems.at(m_actionSids[actionSid].first);
    return menuItem;
}

//-----------------------------------------------------------------------------

void ToolBar::initialize(const ui::config_t& configuration)
{
    // index represents associated toolBar with position in toolBars vector
    unsigned int index = 0;
    m_callbacks.clear();

    // initialize m_actionSids map with configuration

    for(const auto& menuItem : boost::make_iterator_range(configuration.equal_range("menuItem")))
    {
        if(const auto sid = menuItem.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menuItem.second.get("<xmlattr>.start", false);

            SIGHT_ASSERT(
                "The action '" + sid.value() + "' already exists for '" + m_sid + "' menu.",
                m_actionSids.find(sid.value()) == m_actionSids.end()
            );
            m_actionSids[sid.value()] = SIDToolBarMapType::mapped_type(index, start);

            ui::ActionCallbackBase::sptr callback;
            ui::object::sptr guiObj = ui::factory::make(ActionCallbackBase::REGISTRY_KEY);
            callback = std::dynamic_pointer_cast<ui::ActionCallbackBase>(guiObj);
            SIGHT_ASSERT(
                "Cannot create action callback, factory failed for '"
                + ui::ActionCallbackBase::REGISTRY_KEY + "'",
                callback
            );
            callback->setSID(sid.value());
            m_callbacks.push_back(callback);
        }

        index++;
    }

    index = 0;
    // initialize m_menuSids map with configuration
    for(const auto& menu : boost::make_iterator_range(configuration.equal_range("menu")))
    {
        if(const auto sid = menu.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu.second.get("<xmlattr>.start", false);

            SIGHT_ASSERT(
                "Menu '" + sid.value() + "' already exists for '" + m_sid + "' toolBar",
                m_menuSids.find(sid.value()) == m_menuSids.end()
            );

            m_menuSids[sid.value()] = SIDToolBarMapType::mapped_type(index, start);
        }

        index++;
    }

    index = 0;
    // initialize m_menuSids map with configuration
    for(const auto& editor : boost::make_iterator_range(configuration.equal_range("editor")))
    {
        const auto sid = editor.second.get_optional<std::string>("<xmlattr>.sid");
        const auto wid = editor.second.get_optional<std::string>("<xmlattr>.wid");
        SIGHT_ASSERT(
            "[" + m_sid + "] <editor> tag must have sid or wid attribute",
            sid.has_value() || wid.has_value()
        );
        if(sid.has_value())
        {
            const bool start = editor.second.get("<xmlattr>.start", false);

            SIGHT_ASSERT(
                "Editor '" + sid.value() + "' already exists for '" + m_sid + "'  toolBar",
                m_editorSids.find(sid.value()) == m_editorSids.end()
            );
            m_editorSids[sid.value()] = SIDToolBarMapType::mapped_type(index, start);
        }
        else if(wid.has_value())
        {
            SIGHT_ASSERT(
                "Editor '" + wid.value() + "' already exists for '" + m_sid + "'  toolBar",
                m_editorWids.find(wid.value()) == m_editorWids.end()
            );
            m_editorWids[wid.value()] = index;
        }

        index++;
    }
}

//-----------------------------------------------------------------------------

void ToolBar::manage(std::vector<ui::container::menu_item::sptr> menuItems)
{
    ui::container::menu_item::sptr menuItem;
    for(const SIDToolBarMapType::value_type& sid : m_actionSids)
    {
        SIGHT_ASSERT(
            "The toolBar '" << m_sid << "' contains more menuItem in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menuItems in <registry>, but only " << menuItems.size() << " in <layout>.",
            sid.second.first < menuItems.size()
        );
        menuItem = menuItems.at(sid.second.first);
        ui::registry::registerActionSIDToParentSID(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The action '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' toolBar, "
                                                                                                "the service may be created later if it uses deferred objects, thus use start=\"no\" and start it at the end of the configuration",
                core::tools::id::exist(sid.first)
            );
            service::base::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->stopped()
            );
            service->start();
        }
        else
        {
            bool service_exists = core::tools::id::exist(sid.first);
            if(!service_exists || service::get(sid.first)->stopped())
            {
                ui::registry::actionServiceStopping(sid.first);
            }
            else
            {
                ui::registry::actionServiceStarting(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBar::manage(std::vector<ui::container::menu::sptr> menus)
{
    ui::container::menu::sptr menu;
    for(const SIDToolBarMapType::value_type& sid : m_menuSids)
    {
        SIGHT_ASSERT(
            "The toolBar '" << m_sid << "' contains more menu in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menu in <registry>, but only " << menus.size() << "  in <layout>.",
            sid.second.first < menus.size()
        );
        menu = menus.at(sid.second.first);
        ui::registry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' toolbar.",
                core::tools::id::exist(sid.first)
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
}

//-----------------------------------------------------------------------------

void ToolBar::manage(std::vector<ui::container::widget::sptr> containers)
{
    ui::container::widget::sptr container;
    for(const SIDToolBarMapType::value_type& sid : m_editorSids)
    {
        SIGHT_ASSERT(
            "The toolBar '" << m_sid << "' contains more editors in <registry> than in <layout>: "
            << (sid.second.first + 1) << " editors in <registry>, but only " << containers.size() << " in <layout>.",
            sid.second.first < containers.size()
        );
        container = containers.at(sid.second.first);
        ui::registry::registerSIDContainer(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The service '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' toolbar.",
                core::tools::id::exist(sid.first)
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

    for(const WIDToolBarMapType::value_type& wid : m_editorWids)
    {
        SIGHT_ASSERT(
            "The toolBar '" << m_sid << "' contains more editors in <registry> than in <layout>: "
            << (wid.second + 1) << " editors in <registry>, but only " << containers.size() << " in <layout>.",
            wid.second < containers.size()
        );
        container = containers.at(wid.second);
        ui::registry::registerWIDContainer(wid.first, container);
    }
}

//-----------------------------------------------------------------------------

void ToolBar::unmanage()
{
    for(const SIDToolBarMapType::value_type& sid : m_actionSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The toolBar '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                      "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(
                    sid.first
                )
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregisterActionSIDToParentSID(sid.first, m_sid);
    }

    for(const SIDToolBarMapType::value_type& sid : m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The toolBar '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                      "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(
                    sid.first
                )
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregisterSIDMenu(sid.first);
    }

    for(const SIDToolBarMapType::value_type& sid : m_editorSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The toolBar '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                      "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::id::exist(
                    sid.first
                )
            );
            service::base::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::registry::unregisterSIDContainer(sid.first);
    }

    for(const WIDToolBarMapType::value_type& wid : m_editorWids)
    {
        ui::registry::unregisterWIDContainer(wid.first);
    }
}

//-----------------------------------------------------------------------------

void ToolBar::onItemAction()
{
    SIGHT_WARN("TODO: ToolBar::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::detail::registry
