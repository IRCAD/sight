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
#include "fwGui/registrar/ToolBarRegistrar.hpp"

namespace fwGui
{
namespace registrar
{

//-----------------------------------------------------------------------------

ToolBarRegistrar::ToolBarRegistrar(const std::string sid) : m_sid(sid)
{}

//-----------------------------------------------------------------------------

ToolBarRegistrar::~ToolBarRegistrar()
{}

//-----------------------------------------------------------------------------

::fwGui::fwToolBar::sptr ToolBarRegistrar::getParent()
{
    return ::fwGui::GuiRegistry::getSIDToolBar(m_sid);
}

//-----------------------------------------------------------------------------

::fwGui::fwMenuItem::sptr ToolBarRegistrar::getFwMenuItem(std::string actionSid, std::vector< ::fwGui::fwMenuItem::sptr > menuItems)
{
    SLM_ASSERT("toolBarItem not found", m_actionSids.find(actionSid) != m_actionSids.end());
    ::fwGui::fwMenuItem::sptr menuItem = menuItems.at( m_actionSids[actionSid].first );
    return menuItem;
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be registry",
            configuration->getName() == "registry");

    // index represents associated toolBar with position in toolBars vector
    int index = 0;
    m_callbacks.clear();
    // initialize m_actionSids map with configuration
    std::vector < ConfigurationType > vectToolBarItems = configuration->find("toolBarItem");
    BOOST_FOREACH( ConfigurationType toolBarItem, vectToolBarItems)
    {
        SLM_ASSERT("<toolBarItem> tag must have sid attribute", toolBarItem->hasAttribute("sid"));
        if(toolBarItem->hasAttribute("sid"))
        {
            bool start = false;
            if(toolBarItem->hasAttribute("start"))
            {
                std::string startValue = toolBarItem->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                        startValue == "yes" || startValue == "no");
                start = (startValue=="yes");
            }
            std::string sid = toolBarItem->getAttributeValue("sid");
            std::pair<int, bool> indexStart =  std::make_pair( index, start);
            m_actionSids[sid] = indexStart;

            ::fwGui::ActionCallbackBase::sptr callback ;
            callback = ::fwTools::ClassFactoryRegistry::create< ::fwGui::ActionCallbackBase >( ::fwGui::ActionCallbackBase::REGISTRY_KEY );
            OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::ActionCallbackBase::REGISTRY_KEY, callback);

            callback->setSID(sid);
            m_callbacks.push_back(callback);
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::manage(std::vector< ::fwGui::fwMenuItem::sptr > toolBarItems )
{
    ::fwGui::fwMenuItem::sptr toolBarItem;
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_actionSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < toolBarItems.size());
        toolBarItem = toolBarItems.at( sid.second.first );
        ::fwGui::GuiRegistry::registerActionSIDToParentSID(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::UUID::exist(sid.first, ::fwTools::UUID::SIMPLE ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::unmanage()
{
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_actionSids)
    {
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::UUID::exist(sid.first, ::fwTools::UUID::SIMPLE ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterActionSIDToParentSID(sid.first, m_sid);
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::onItemAction()
{
    SLM_WARN("TODO: ToolBarRegistrar::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
