/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwGui/registrar/ViewRegistrar.hpp"

#include "fwGui/container/fwContainer.hpp"
#include "fwGui/container/fwMenuBar.hpp"
#include "fwGui/container/fwToolBar.hpp"
#include "fwGui/GuiRegistry.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <utility>

namespace fwGui
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

::fwGui::container::fwContainer::sptr ViewRegistrar::getParent()
{
    ::fwGui::container::fwContainer::sptr parentContainer;
    if(!m_parentWid.empty())
    {
        parentContainer = ::fwGui::GuiRegistry::getWIDContainer(m_parentWid);
        SLM_ASSERT("The parent view of the wid container '"+ m_parentWid +"' is not found. Check that the view is "
                   "properly declared and the service is started.",
                   parentContainer);
    }
    else
    {
        parentContainer = ::fwGui::GuiRegistry::getSIDContainer(m_sid);
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

void ViewRegistrar::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
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

void ViewRegistrar::manage(std::vector< ::fwGui::container::fwContainer::sptr > subViews )
{
    ::fwGui::container::fwContainer::sptr container;
    for( SIDContainerMapType::value_type sid :  m_sids)
    {
        OSLM_ASSERT("The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
                                 << (sid.second.first+1) << " views in <registry>, but only " << subViews.size() <<" in <layout>.",
                    sid.second.first < subViews.size());
        container = subViews.at( sid.second.first );
        ::fwGui::GuiRegistry::registerSIDContainer(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("The service '"+sid.first +"' does not exist, but is declared in '" + m_sid + "' view, "
                        "the service may be created later if it uses deferred objects, thus use start=\"no\" and start "
                        "it at the end of the configuration",
                        ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first );
            SLM_ASSERT("The service '"+sid.first +"' cannot be started by '" + m_sid + "' because it is not stopped."
                       , service->isStopped() );
            SLM_TRACE("The service '" + sid.first + "' is started by '" + m_sid + "'");
            service->start();
        }
    }

    for( WIDContainerMapType::value_type wid :  m_wids)
    {
        OSLM_ASSERT("The view '" << m_sid << "' contains more sub-views in <registry> than in <layout>: "
                                 << (wid.second+1) << " views in <registry>, but only " << subViews.size() <<" in <layout>.",
                    wid.second < subViews.size());
        container = subViews.at( wid.second );
        ::fwGui::GuiRegistry::registerWIDContainer(wid.first, container);
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::manageMenuBar(::fwGui::container::fwMenuBar::sptr menuBar )
{
    ::fwGui::GuiRegistry::registerSIDMenuBar(m_menuBarSid.first, menuBar);
    if(m_menuBarSid.second) //service is auto started?
    {
        SLM_ASSERT("The menuBar service '"+m_menuBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                   ::fwTools::fwID::exist(m_menuBarSid.first ) );
        ::fwServices::IService::sptr service = ::fwServices::get( m_menuBarSid.first );
        service->start();
    }
}

//-----------------------------------------------------------------------------

void ViewRegistrar::manageToolBar(::fwGui::container::fwToolBar::sptr toolBar )
{
    ::fwGui::GuiRegistry::registerSIDToolBar(m_toolBarSid.first, toolBar);
    if(m_toolBarSid.second) //service is auto started?
    {
        SLM_ASSERT("The toolBar service '"+m_toolBarSid.first +"' declared by '" + m_sid + "' does not exist.",
                   ::fwTools::fwID::exist(m_toolBarSid.first ) );
        ::fwServices::IService::sptr service = ::fwServices::get( m_toolBarSid.first );
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
            OSLM_ASSERT("The view '" + m_sid + "' try to stop the service '" + sid.first + "' but it does not exist. "
                        "It may have been destroyed by the configuration if it uses deferred objects.",
                        ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first );
            SLM_TRACE("The service '" + sid.first + "' is stopped by '" + m_sid + "'");
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDContainer(sid.first);
    }

    for( WIDContainerMapType::value_type wid :  m_wids)
    {
        ::fwGui::GuiRegistry::unregisterWIDContainer(wid.first);
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
                       ::fwTools::fwID::exist(m_toolBarSid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( m_toolBarSid.first );
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDToolBar(m_toolBarSid.first);
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
                       ::fwTools::fwID::exist(m_menuBarSid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( m_menuBarSid.first );
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDMenuBar(m_menuBarSid.first);
    }
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
