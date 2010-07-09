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
#include "fwGui/registrar/MenuRegistrar.hpp"

namespace fwGui
{
namespace registrar
{

//-----------------------------------------------------------------------------

MenuRegistrar::MenuRegistrar(const std::string sid) : m_sid(sid)
{}

//-----------------------------------------------------------------------------

MenuRegistrar::~MenuRegistrar()
{}

//-----------------------------------------------------------------------------

::fwGui::fwMenu::sptr MenuRegistrar::getParent()
{
    return ::fwGui::GuiRegistry::getSIDMenu(m_sid);
}

//-----------------------------------------------------------------------------

::fwGui::fwMenuItem::sptr MenuRegistrar::getFwMenuItem(std::string actionSid, std::vector< ::fwGui::fwMenuItem::sptr > menuItems)
{
    SLM_ASSERT("menuItem not found", m_actionSids.find(actionSid) != m_actionSids.end());
    ::fwGui::fwMenuItem::sptr menuItem = menuItems.at( m_actionSids[actionSid].first );
    return menuItem;
}

//-----------------------------------------------------------------------------

void MenuRegistrar::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be registry",
            configuration->getName() == "registry");

    // index represents associated menu with position in menus vector
    int index = 0;
    m_callbacks.clear();
    // initialize m_actionSids map with configuration
    std::vector < ConfigurationType > vectMenuItems = configuration->find("menuItem");
    BOOST_FOREACH( ConfigurationType menuItem, vectMenuItems)
    {
        SLM_ASSERT("<menuItem> tag must have sid attribute", menuItem->hasAttribute("sid"));
        if(menuItem->hasAttribute("sid"))
        {
            bool start = false;
            if(menuItem->hasAttribute("start"))
            {
                std::string startValue = menuItem->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                        startValue == "yes" || startValue == "no");
                start = (startValue=="yes");
            }
            std::string sid = menuItem->getAttributeValue("sid");
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

void MenuRegistrar::manage(std::vector< ::fwGui::fwMenuItem::sptr > menuItems )
{
    ::fwGui::fwMenuItem::sptr menuItem;
    BOOST_FOREACH( SIDMenuMapType::value_type sid, m_actionSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < menuItems.size());
        menuItem = menuItems.at( sid.second.first );
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

void MenuRegistrar::unmanage()
{
    BOOST_FOREACH( SIDMenuMapType::value_type sid, m_actionSids)
    {
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::UUID::exist(sid.first, ::fwTools::UUID::SIMPLE ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterActionSIDToParentSID(sid.first);
    }
}

//-----------------------------------------------------------------------------

void MenuRegistrar::onItemAction()
{
    SLM_WARN("TODO: MenuRegistrar::onItemAction not yet implemented");
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
