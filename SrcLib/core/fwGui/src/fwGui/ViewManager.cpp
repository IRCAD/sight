/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/GuiRegistry.hpp"
#include "fwGui/ViewManager.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

ViewManager::ViewManager(const std::string sid) : m_sid(sid)
{}

//-----------------------------------------------------------------------------

ViewManager::~ViewManager()
{}

//-----------------------------------------------------------------------------

::fwGui::fwContainer::sptr ViewManager::getParent()
{
    return this->m_parentContainer;
}

//-----------------------------------------------------------------------------

void ViewManager::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be viewManager",
            configuration->getName() == "viewManager");

    // find parent container
    std::vector < ConfigurationType > vectParent = configuration->find("parent");
    if(!vectParent.empty())
    {
        ConfigurationType parent = vectParent.at(0);
        SLM_ASSERT("<parent> tag must have wid attribute", parent->hasAttribute("wid"));
        std::string wid = parent->getAttributeValue("wid");
        this->m_parentContainer = ::fwGui::GuiRegistry::getWIDContainer(wid);
    }
    else
    {
        this->m_parentContainer = ::fwGui::GuiRegistry::getSIDContainer(m_sid);
    }
    // index represents associated container with position in subViews vector
    int index = 0;
    // initialize m_sids and m_wids map with configuration
    std::vector < ConfigurationType > vectViews = configuration->find("view");
    BOOST_FOREACH( ConfigurationType view, vectViews)
    {
        SLM_ASSERT("<view> tag must have sid or wid attribute",
                view->hasAttribute("sid") || view->hasAttribute("wid"));
        SLM_ASSERT("<view> tag must have index attribute",
                        view->hasAttribute("index"));
        if(view->hasAttribute("sid"))
        {
            bool start = false;
            if(view->hasAttribute("start"))
            {
                std::string startValue = view->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                        startValue == "yes" || startValue == "no");
                start = (startValue=="yes");
            }
            std::string sid = view->getAttributeValue("sid");
            std::pair<int, bool> indexStart =  std::make_pair( index, start);
            m_sids[sid] = indexStart;
        }
        else if(view->hasAttribute("wid"))
        {
            std::string wid = view->getAttributeValue("wid");
            m_wids[wid] = index;
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void ViewManager::manage(std::vector< ::fwGui::fwContainer::sptr > subViews )
{
    ::fwGui::fwContainer::sptr container;
    BOOST_FOREACH( SIDContainerMapType::value_type sid, m_sids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < subViews.size());
        container = subViews.at( sid.second.first );
        ::fwGui::GuiRegistry::registerSIDContainer(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::UUID::exist(sid.first, ::fwTools::UUID::SIMPLE ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->start();
        }
    }

    BOOST_FOREACH( WIDContainerMapType::value_type wid, m_wids)
    {
        OSLM_ASSERT("Container index "<< wid.second <<" is bigger than subViews size!", wid.second < subViews.size());
        container = subViews.at( wid.second );
        ::fwGui::GuiRegistry::registerWIDContainer(wid.first, container);
    }
}

//-----------------------------------------------------------------------------

void ViewManager::unmanage()
{
    BOOST_FOREACH( SIDContainerMapType::value_type sid, m_sids)
    {
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::UUID::exist(sid.first, ::fwTools::UUID::SIMPLE ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDContainer(sid.first);
    }

    BOOST_FOREACH( WIDContainerMapType::value_type wid, m_wids)
    {
        ::fwGui::GuiRegistry::unregisterWIDContainer(wid.first);
    }
}

//-----------------------------------------------------------------------------

} //namespace fwGui
