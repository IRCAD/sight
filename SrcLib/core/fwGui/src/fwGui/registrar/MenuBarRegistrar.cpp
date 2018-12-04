/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGui/registrar/MenuBarRegistrar.hpp"

#include "fwGui/GuiRegistry.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

#include <utility>

namespace fwGui
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

::fwGui::container::fwMenuBar::sptr MenuBarRegistrar::getParent()
{
    return ::fwGui::GuiRegistry::getSIDMenuBar(m_sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenu::sptr MenuBarRegistrar::getFwMenu(std::string menuSid,
                                                             std::vector< ::fwGui::container::fwMenu::sptr > menus)
{
    SLM_ASSERT("menu not found", m_menuSids.find(menuSid) != m_menuSids.end());
    ::fwGui::container::fwMenu::sptr menu = menus.at( m_menuSids[menuSid].first );
    return menu;
}

//-----------------------------------------------------------------------------

void MenuBarRegistrar::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be registry",
                configuration->getName() == "registry");

    // index represents associated menu with position in menus vector
    unsigned int index = 0;
    // initialize m_menuSids map with configuration
    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    for( ConfigurationType menu :  vectMenus)
    {
        SLM_ASSERT("<menu> tag must have sid attribute", menu->hasAttribute("sid"));

        if(menu->hasAttribute("sid"))
        {
            bool start = false;
            if(menu->hasAttribute("start"))
            {
                std::string startValue = menu->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                           startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = menu->getAttributeValue("sid");
            m_menuSids[sid] = SIDMenuMapType::mapped_type(index, start);
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void MenuBarRegistrar::manage(std::vector< ::fwGui::container::fwMenu::sptr > menus )
{
    ::fwGui::container::fwMenu::sptr menu;
    for( SIDMenuMapType::value_type sid :  m_menuSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!",
                    sid.second.first < menus.size());
        menu = menus.at( sid.second.first );
        ::fwGui::GuiRegistry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" does not exist.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first );
            service->start();
        }
        else
        {
            bool service_exists = ::fwTools::fwID::exist(sid.first );
            if (!service_exists || ::fwServices::get( sid.first )->isStopped())
            {
                ::fwGui::GuiRegistry::actionServiceStopping(sid.first);
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
            OSLM_ASSERT("Service "<<sid.first <<" does not exist.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first );
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDMenu(sid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
