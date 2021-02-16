/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/registrar/MenuRegistrar.hpp"

#include "ui/base/GuiRegistry.hpp"
#include "ui/base/IAction.hpp"

#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <utility>

namespace sight::ui::base
{
namespace registrar
{

//-----------------------------------------------------------------------------

MenuRegistrar::MenuRegistrar(const std::string& sid) :
    m_sid(sid)
{
}

//-----------------------------------------------------------------------------

MenuRegistrar::~MenuRegistrar()
{
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenu::sptr MenuRegistrar::getParent()
{
    return ui::base::GuiRegistry::getSIDMenu(m_sid);
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenuItem::sptr MenuRegistrar::getFwMenuItem(std::string actionSid,
                                                                   std::vector< ui::base::container::fwMenuItem::sptr > menuItems)
{
    SLM_ASSERT("The action '" + actionSid + "' declared by the Menu '" + m_sid + "' is not found",
               m_actionSids.find(actionSid) != m_actionSids.end());
    ui::base::container::fwMenuItem::sptr menuItem = menuItems.at( m_actionSids[actionSid].first );
    return menuItem;
}

//-----------------------------------------------------------------------------

void MenuRegistrar::initialize( core::runtime::ConfigurationElement::sptr configuration)
{
    SLM_ASSERT(
        "Wrong configuration name for '" + m_sid + "', expected 'registry', actual: '" + configuration->getName()+ "'",
        configuration->getName() == "registry");

    // index represents associated menu with position in menus vector
    unsigned int index = 0;
    m_callbacks.clear();
    // initialize m_actionSids map with configuration
    std::vector < ConfigurationType > vectMenuItems = configuration->find("menuItem");
    for( ConfigurationType menuItem :  vectMenuItems)
    {
        SLM_ASSERT("[" + m_sid + "] <menuItem> tag must have 'sid' attribute", menuItem->hasAttribute("sid"));
        if(menuItem->hasAttribute("sid"))
        {
            bool start = false;
            if(menuItem->hasAttribute("start"))
            {
                std::string startValue = menuItem->getAttributeValue("start");
                SLM_ASSERT("[" + m_sid + "] Wrong value for 'start' attribute (requires 'yes' or 'no'), actual: '"
                           + startValue +"'.", startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = menuItem->getAttributeValue("sid");
            SLM_ASSERT("The action '" + sid + "' already exists for '" + m_sid + "' menu.",
                       m_actionSids.find( sid) == m_actionSids.end());
            m_actionSids[sid] = SIDMenuMapType::mapped_type(index, start);

            ui::base::ActionCallbackBase::sptr callback;
            ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ActionCallbackBase::REGISTRY_KEY);
            callback = ui::base::ActionCallbackBase::dynamicCast(guiObj);
            SLM_ASSERT("Cannot create action callback, factory failed for '"
                       + ui::base::ActionCallbackBase::REGISTRY_KEY + "'", callback);

            callback->setSID(sid);
            m_callbacks.push_back(callback);
        }
        index++;
    }

    index = 0;
    // initialize m_actionSids map with configuration
    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    for( ConfigurationType menu :  vectMenus)
    {
        SLM_ASSERT("[" + m_sid + "] <menu> tag must have sid attribute", menu->hasAttribute("sid"));
        if(menu->hasAttribute("sid"))
        {
            bool start = false;
            if(menu->hasAttribute("start"))
            {
                std::string startValue = menu->getAttributeValue("start");
                SLM_ASSERT("[" + m_sid + "] Wrong value for 'start' attribute (require yes or no), actual: '"
                           << startValue <<"'.", startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid                 = menu->getAttributeValue("sid");
            std::pair<int, bool> indexStart = std::make_pair( index, start);
            SLM_ASSERT("The menu '" + sid + "' already exists for this menu '" + m_sid + "'",
                       m_actionSids.find( sid) == m_actionSids.end());
            m_menuSids[sid] = indexStart;
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void MenuRegistrar::manage(std::vector< ui::base::container::fwMenuItem::sptr > menuItems )
{
    ui::base::container::fwMenuItem::sptr menuItem;
    for( SIDMenuMapType::value_type sid :  m_actionSids)
    {
        SLM_ASSERT("The menu '" << m_sid << "' contains more menuItems in <registry> than in <layout>: "
                                << (sid.second.first+1) << " menuItems in <registry>, but only " << menuItems.size() <<" in <layout>.",
                   sid.second.first < menuItems.size());
        menuItem = menuItems.at( sid.second.first );
        ui::base::GuiRegistry::registerActionSIDToParentSID(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The action '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' menu, "
                       "the service may be created later if it uses deferred objects, thus use start=\"no\" and start it at the end of the configuration ",
                       core::tools::fwID::exist(sid.first ));
            service::IService::sptr service = service::get( sid.first );
            SLM_ASSERT("The service '"+sid.first +"' cannot be started by '" + m_sid + "' because it is not stopped."
                       , service->isStopped() );
            service->start();
        }
        else
        {
            bool service_exists = core::tools::fwID::exist(sid.first );
            if (!service_exists || service::get( sid.first )->isStopped())
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

void MenuRegistrar::manage(std::vector< ui::base::container::fwMenu::sptr > menus )
{
    ui::base::container::fwMenu::sptr menu;
    for( SIDMenuMapType::value_type sid :  m_menuSids)
    {
        SLM_ASSERT("The menu '" << m_sid << "' contains more menus in <registry> than in <layout>: "
                                << sid.second.first << " menus in <registry>, but only " << menus.size() <<" in <layout>.",
                   (sid.second.first+1) < menus.size());
        menu = menus.at( sid.second.first );
        ui::base::GuiRegistry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The menu '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' menu.",
                       core::tools::fwID::exist(sid.first ));
            service::IService::sptr service = service::get( sid.first );
            SLM_ASSERT("The service '"+sid.first +"' cannot be started by '" + m_sid + "' because it is not stopped."
                       , service->isStopped() );
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void MenuRegistrar::unmanage()
{
    for( SIDMenuMapType::value_type sid :  m_actionSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                       "It may have been destroyed by the configuration if it uses deferred objects.",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterActionSIDToParentSID(sid.first, m_sid);
    }
    for( SIDMenuMapType::value_type sid :  m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The menu '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                       "It may have been destroyed by the configuration if it uses deferred objects.",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterSIDMenu(sid.first);
    }
}

//-----------------------------------------------------------------------------

void MenuRegistrar::onItemAction()
{
    SLM_WARN("TODO: MenuRegistrar::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace sight::ui::base
