/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "Menu.hpp"

#include "ui/base/GuiRegistry.hpp"
#include "ui/base/IAction.hpp"

#include <service/op/Get.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <utility>

namespace sight::ui::base::registry
{

//-----------------------------------------------------------------------------

Menu::Menu(std::string sid) :
    m_sid(std::move(sid))
{
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenu::sptr Menu::getParent()
{
    return ui::base::GuiRegistry::getSIDMenu(m_sid);
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenuItem::sptr Menu::getFwMenuItem(
    std::string actionSid,
    std::vector<ui::base::container::fwMenuItem::sptr> menuItems
)
{
    SIGHT_ASSERT(
        "The action '" + actionSid + "' declared by the Menu '" + m_sid + "' is not found",
        m_actionSids.find(actionSid) != m_actionSids.end()
    );
    ui::base::container::fwMenuItem::sptr menuItem = menuItems.at(m_actionSids[actionSid].first);
    return menuItem;
}

//-----------------------------------------------------------------------------

void Menu::initialize(const ui::base::config_t& configuration)
{
    // index represents associated menu with position in menus vector
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
            m_actionSids[sid.value()] = SIDMenuMapType::mapped_type(index, start);

            ui::base::ActionCallbackBase::sptr callback;
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ActionCallbackBase::REGISTRY_KEY);
            callback = ui::base::ActionCallbackBase::dynamicCast(guiObj);
            SIGHT_ASSERT(
                "Cannot create action callback, factory failed for '"
                + ui::base::ActionCallbackBase::REGISTRY_KEY + "'",
                callback
            );

            callback->setSID(sid.value());
            m_callbacks.push_back(callback);
        }

        index++;
    }

    index = 0;
    // initialize m_actionSids map with configuration
    for(const auto& menu : boost::make_iterator_range(configuration.equal_range("menu")))
    {
        if(const auto sid = menu.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
        {
            const bool start = menu.second.get("<xmlattr>.start", false);
            SIGHT_ASSERT(
                "The menu '" + sid.value() + "' already exists for this menu '" + m_sid + "'",
                m_menuSids.find(sid.value()) == m_menuSids.end()
            );
            m_menuSids[sid.value()] = SIDMenuMapType::mapped_type(index, start);
        }

        index++;
    }
}

//-----------------------------------------------------------------------------

void Menu::manage(std::vector<ui::base::container::fwMenuItem::sptr> menuItems)
{
    ui::base::container::fwMenuItem::sptr menuItem;
    for(const SIDMenuMapType::value_type& sid : m_actionSids)
    {
        SIGHT_ASSERT(
            "The menu '" << m_sid << "' contains more menuItems in <registry> than in <layout>: "
            << (sid.second.first + 1) << " menuItems in <registry>, but only " << menuItems.size() << " in <layout>.",
            sid.second.first < menuItems.size()
        );
        menuItem = menuItems.at(sid.second.first);
        ui::base::GuiRegistry::registerActionSIDToParentSID(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The action '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menu, "
                                                                                                "the service may be created later if it uses deferred objects, thus use start=\"no\" and start it at the end of the configuration ",
                core::tools::fwID::exist(sid.first)
            );
            service::IService::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->isStopped()
            );
            service->start();
        }
        else
        {
            bool service_exists = core::tools::fwID::exist(sid.first);
            if(!service_exists || service::get(sid.first)->isStopped())
            {
                ui::base::GuiRegistry::actionServiceStopping(sid.first);
            }
            else
            {
                ui::base::GuiRegistry::actionServiceStarting(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Menu::manage(std::vector<ui::base::container::fwMenu::sptr> menus)
{
    ui::base::container::fwMenu::sptr menu;
    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        SIGHT_ASSERT(
            "The menu '" << m_sid << "' contains more menus in <registry> than in <layout>: "
            << sid.second.first << " menus in <registry>, but only " << menus.size() << " in <layout>.",
            (sid.second.first + 1) < menus.size()
        );
        menu = menus.at(sid.second.first);
        ui::base::GuiRegistry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + sid.first + "' does not exist, but is declared in '" + m_sid + "' menu.",
                core::tools::fwID::exist(sid.first)
            );
            service::IService::sptr service = service::get(sid.first);
            SIGHT_ASSERT(
                "The service '" + sid.first + "' cannot be started by '" + m_sid + "' because it is not stopped."
                ,
                service->isStopped()
            );
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void Menu::unmanage()
{
    for(const SIDMenuMapType::value_type& sid : m_actionSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                   "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::fwID::exist(sid.first)
            );
            service::IService::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::base::GuiRegistry::unregisterActionSIDToParentSID(sid.first, m_sid);
    }

    for(const SIDMenuMapType::value_type& sid : m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SIGHT_ASSERT(
                "The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                                                                                   "It may have been destroyed by the configuration if it uses deferred objects.",
                core::tools::fwID::exist(sid.first)
            );
            service::IService::sptr service = service::get(sid.first);
            service->stop().wait();
        }

        ui::base::GuiRegistry::unregisterSIDMenu(sid.first);
    }
}

//-----------------------------------------------------------------------------

void Menu::onItemAction()
{
    SIGHT_WARN("TODO: Menu::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::registry
