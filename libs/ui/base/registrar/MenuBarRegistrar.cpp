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

#include "ui/base/registrar/MenuBarRegistrar.hpp"

#include "ui/base/GuiRegistry.hpp"

#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <utility>

namespace sight::ui::base
{
namespace registrar
{

//-----------------------------------------------------------------------------

MenuBarRegistrar::MenuBarRegistrar(const std::string& sid) :
    m_sid(sid)
{
}

//-----------------------------------------------------------------------------

MenuBarRegistrar::~MenuBarRegistrar()
{
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenuBar::sptr MenuBarRegistrar::getParent()
{
    return ui::base::GuiRegistry::getSIDMenuBar(m_sid);
}

//-----------------------------------------------------------------------------

ui::base::container::fwMenu::sptr MenuBarRegistrar::getFwMenu(std::string menuSid,
                                                              std::vector< ui::base::container::fwMenu::sptr > menus)
{
    SLM_ASSERT("The menu '" + menuSid + "'declared in '"+ m_sid +"' is not found",
               m_menuSids.find(menuSid) != m_menuSids.end());
    ui::base::container::fwMenu::sptr menu = menus.at( m_menuSids[menuSid].first );
    return menu;
}

//-----------------------------------------------------------------------------

void MenuBarRegistrar::initialize( core::runtime::ConfigurationElement::sptr configuration)
{
    SLM_ASSERT(
        "Wrong configuration name for '" + m_sid + "', expected 'registry', actual: '" + configuration->getName()+ "'",
        configuration->getName() == "registry");

    // index represents associated menu with position in menus vector
    unsigned int index = 0;
    // initialize m_menuSids map with configuration
    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    for( ConfigurationType menu :  vectMenus)
    {
        SLM_ASSERT("[" + m_sid + "] <menu> tag must have 'sid' attribute", menu->hasAttribute("sid"));

        if(menu->hasAttribute("sid"))
        {
            bool start = false;
            if(menu->hasAttribute("start"))
            {
                std::string startValue = menu->getAttributeValue("start");
                SLM_ASSERT("[" + m_sid + "] Wrong value for 'start' attribute (requires 'yes' or 'no'), actual: "
                           + startValue +"'", startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = menu->getAttributeValue("sid");
            m_menuSids[sid] = SIDMenuMapType::mapped_type(index, start);
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void MenuBarRegistrar::manage(std::vector< ui::base::container::fwMenu::sptr > menus )
{
    ui::base::container::fwMenu::sptr menu;
    for( SIDMenuMapType::value_type sid :  m_menuSids)
    {
        SLM_ASSERT("The menuBar '" << m_sid << "' contains more menus in <registry> than in <layout>: "
                                   << (sid.second.first+1) << " menus in <registry>, but only " << menus.size() <<" in <layout>.",
                   sid.second.first < menus.size());
        menu = menus.at( sid.second.first );
        ui::base::GuiRegistry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("Menu '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' menuBar.",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            service->start();
        }
        else
        {
            bool service_exists = core::tools::fwID::exist(sid.first );
            if (!service_exists || service::get( sid.first )->isStopped())
            {
                ui::base::GuiRegistry::actionServiceStopping(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void MenuBarRegistrar::unmanage()
{
    for( SIDMenuMapType::value_type sid :  m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("Menu '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' menuBar.",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterSIDMenu(sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace sight::ui::base
