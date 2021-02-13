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

#include "ui/base/registrar/ViewRegistrar.hpp"

#include "ui/base/container/fwContainer.hpp"
#include "ui/base/container/fwMenuBar.hpp"
#include "ui/base/container/fwToolBar.hpp"
#include "ui/base/GuiRegistry.hpp"

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <utility>

namespace sight::ui::base
{
namespace registrar
{

//-----------------------------------------------------------------------------

ViewRegistrar::ViewRegistrar(const std::string& sid) :
    m_parentWid(""),
    m_sid(sid)
{
}

//-----------------------------------------------------------------------------

ViewRegistrar::~ViewRegistrar()
{
}

//-----------------------------------------------------------------------------

ui::base::container::fwContainer::sptr ViewRegistrar::getParent()
{
    ui::base::container::fwContainer::sptr parentContainer;
    if(!m_parentWid.empty())
    {
        parentContainer = ui::base::GuiRegistry::getWIDContainer(m_parentWid);
        SLM_ASSERT("The parent view of the wid container '"+ m_parentWid +"' is not found. Check that the view is "
                   "properly declared and the service is started.",
                   parentContainer);
    }
    else
    {
        parentContainer = ui::base::GuiRegistry::getSIDContainer(m_sid);
        SLM_ASSERT("The parent view of the service '"+ m_sid +"' is not found. Check that the view is "
                   "properly declared and the service is started.",
                   parentContainer );
    }
    return parentContainer;
}

//-----------------------------------------------------------------------------

void ViewRegistrar::setParent(std::string wid)
{
    SLM_ASSERT("The method 'setParent()' is available only if this service declares its parent container with a 'wid'.",
               !m_parentWid.empty());
    m_parentWid = wid;
}

//-----------------------------------------------------------------------------

void ViewRegistrar::initialize( core::runtime::ConfigurationElement::sptr configuration)
{
    SLM_ASSERT(
        "Wrong configuration name for '" + m_sid + "', expected 'registry', actual: '" + configuration->getName()+ "'",
        configuration->getName() == "registry");

    // find parent container
    std::vector < ConfigurationType > vectParent = configuration->find("parent");
    if(!vectParent.empty())
    {
        ConfigurationType parent = vectParent.at(0);
        SLM_ASSERT("<parent> tag must have a 'wid' attribute", parent->hasAttribute("wid"));
        m_parentWid = parent->getAttributeValue("wid");
    }

    // index represents associated container with position in subViews vector
    unsigned int index = 0;
    // initialize m_sids and m_wids map with configuration
    std::vector < ConfigurationType > vectViews      = configuration->find("view");
    std::vector < ConfigurationType > vectSlideViews = configuration->find("slideView");
    std::copy(vectSlideViews.begin(), vectSlideViews.end(), std::back_inserter(vectViews));

    for( ConfigurationType view :  vectViews)
    {
        SLM_ASSERT("<view> tag must have sid or wid attribute",
                   view->hasAttribute("sid") || view->hasAttribute("wid"));
        if(view->hasAttribute("sid"))
        {
            bool start = false;
            if(view->hasAttribute("start"))
            {
                std::string startValue = view->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                           startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = view->getAttributeValue("sid");
            m_sids[sid] = SIDContainerMapType::mapped_type(index, start);
        }
        else if(view->hasAttribute("wid"))
        {
            std::string wid = view->getAttributeValue("wid");
            m_wids[wid] = index;
        }
        index++;
    }

    // find menuBar
    std::vector < ConfigurationType > vectmenuBar = configuration->find("menuBar");
    if(!vectmenuBar.empty())
    {
        ConfigurationType menuBarCfg = vectmenuBar.at(0);
        if (menuBarCfg->hasAttribute("sid"))
        {
            bool start = false;
            if (menuBarCfg->hasAttribute("start"))
            {
                std::string startValue = menuBarCfg->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                           startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = menuBarCfg->getAttributeValue("sid");
            m_menuBarSid = std::make_pair( sid, start);
        }
    }

    // find toolBar
    std::vector < ConfigurationType > vectToolBar = configuration->find("toolBar");
    if(!vectToolBar.empty())
    {
        ConfigurationType toolBarCfg = vectToolBar.at(0);
        if (toolBarCfg->hasAttribute("sid"))
        {
            bool start = false;
            if (toolBarCfg->hasAttribute("start"))
            {
                std::string startValue = toolBarCfg->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                           startValue == "yes" || startValue == "no");
                start = (startValue == "yes");
            }
            std::string sid = toolBarCfg->getAttributeValue("sid");
            m_toolBarSid = std::make_pair( sid, start);
        }
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::manage(std::vector< ui::base::container::fwContainer::sptr > subViews )
{
    ui::base::container::fwContainer::sptr container;
    for( SIDContainerMapType::value_type sid :  m_sids)
    {
        SLM_ASSERT("The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
                                << (sid.second.first+1) << " views in <registry>, but only " << subViews.size() <<" in <layout>.",
                   sid.second.first < subViews.size());
        container = subViews.at( sid.second.first );
        ui::base::GuiRegistry::registerSIDContainer(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The service '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' view, "
                       "the service may be created later if it uses deferred objects, thus use start=\"no\" and start "
                       "it at the end of the configuration",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            SLM_ASSERT("The service '"+sid.first +"' cannot be started by '" + m_sid + "' because it is not stopped."
                       , service->isStopped() );
            service->start();
        }
    }

    for( WIDContainerMapType::value_type wid :  m_wids)
    {
        SLM_ASSERT("The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
                                << (wid.second+1) << " views in <registry>, but only " << subViews.size() <<" in <layout>.",
                   wid.second < subViews.size());
        container = subViews.at( wid.second );
        ui::base::GuiRegistry::registerWIDContainer(wid.first, container);
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::manageMenuBar(ui::base::container::fwMenuBar::sptr menuBar )
{
    ui::base::GuiRegistry::registerSIDMenuBar(m_menuBarSid.first, menuBar);
    if(m_menuBarSid.second) //service is auto started?
    {
        SLM_ASSERT("The menuBar service '"+m_menuBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                   core::tools::fwID::exist(m_menuBarSid.first ) );
        service::IService::sptr service = service::get( m_menuBarSid.first );
        service->start();
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::manageToolBar(ui::base::container::fwToolBar::sptr toolBar )
{
    ui::base::GuiRegistry::registerSIDToolBar(m_toolBarSid.first, toolBar);
    if(m_toolBarSid.second) //service is auto started?
    {
        SLM_ASSERT("The toolBar service '"+m_toolBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                   core::tools::fwID::exist(m_toolBarSid.first ) );
        service::IService::sptr service = service::get( m_toolBarSid.first );
        service->start();
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::unmanage()
{
    for( SIDContainerMapType::value_type sid :  m_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            SLM_ASSERT("The view '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                       "It may have been destroyed by the configuration if it uses deferred objects.",
                       core::tools::fwID::exist(sid.first ) );
            service::IService::sptr service = service::get( sid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterSIDContainer(sid.first);
    }

    for( WIDContainerMapType::value_type wid :  m_wids)
    {
        ui::base::GuiRegistry::unregisterWIDContainer(wid.first);
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::unmanageToolBar()
{
    if ( !m_toolBarSid.first.empty() )
    {
        if(m_toolBarSid.second) //service is auto started?
        {
            SLM_ASSERT("The toolBar service '"+m_toolBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                       core::tools::fwID::exist(m_toolBarSid.first ) );
            service::IService::sptr service = service::get( m_toolBarSid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterSIDToolBar(m_toolBarSid.first);
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::unmanageMenuBar()
{
    if ( !m_menuBarSid.first.empty() )
    {
        if(m_menuBarSid.second) //service is auto started?
        {
            SLM_ASSERT("The menuBar service '"+m_menuBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                       core::tools::fwID::exist(m_menuBarSid.first ) );
            service::IService::sptr service = service::get( m_menuBarSid.first );
            service->stop().wait();
        }
        ui::base::GuiRegistry::unregisterSIDMenuBar(m_menuBarSid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace sight::ui::base
