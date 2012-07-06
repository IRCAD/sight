/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>
#include <fwServices/Base.hpp>

#include "fwGui/GuiRegistry.hpp"
#include "fwGui/IActionSrv.hpp"
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

::fwGui::container::fwToolBar::sptr ToolBarRegistrar::getParent()
{
    return ::fwGui::GuiRegistry::getSIDToolBar(m_sid);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenuItem::sptr ToolBarRegistrar::getFwMenuItem(std::string actionSid, std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems)
{
    SLM_ASSERT("menuItem not found", m_actionSids.find(actionSid) != m_actionSids.end());
    ::fwGui::container::fwMenuItem::sptr menuItem = menuItems.at( m_actionSids[actionSid].first );
    return menuItem;
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be registry",
            configuration->getName() == "registry");

    // index represents associated toolBar with position in toolBars vector
    unsigned int index = 0;
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
            OSLM_ASSERT("Action " << sid << " already exists for this toolBar", m_actionSids.find(sid) == m_actionSids.end());
            m_actionSids[sid] = SIDToolBarMapType::mapped_type(index, start);

            ::fwGui::ActionCallbackBase::sptr callback ;
            callback = ::fwTools::ClassFactoryRegistry::create< ::fwGui::ActionCallbackBase >( ::fwGui::ActionCallbackBase::REGISTRY_KEY );
            OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::ActionCallbackBase::REGISTRY_KEY, callback);

            callback->setSID(sid);
            m_callbacks.push_back(callback);
        }
        index++;
    }
    index = 0;
    // initialize m_menuSids map with configuration
    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    BOOST_FOREACH( ConfigurationType menu, vectMenus)
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
                start = (startValue=="yes");
            }
            std::string sid = menu->getAttributeValue("sid");
            OSLM_ASSERT("Action " << sid << " already exists for this toolBar", m_menuSids.find(sid) == m_menuSids.end());
            m_menuSids[sid] = SIDToolBarMapType::mapped_type(index, start);
        }
        index++;
    }

    index = 0;
    // initialize m_menuSids map with configuration
    std::vector < ConfigurationType > vectEditors = configuration->find("editor");
    BOOST_FOREACH( ConfigurationType editor, vectEditors)
    {
        SLM_ASSERT("<editor> tag must have sid attribute", editor->hasAttribute("sid"));
        if(editor->hasAttribute("sid"))
        {
            bool start = false;
            if(editor->hasAttribute("start"))
            {
                std::string startValue = editor->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                        startValue == "yes" || startValue == "no");
                start = (startValue=="yes");
            }
            std::string sid = editor->getAttributeValue("sid");
            OSLM_ASSERT("Action " << sid << " already exists for this toolBar", m_editorSids.find(sid) == m_editorSids.end());
            m_editorSids[sid] = SIDToolBarMapType::mapped_type(index, start);
        }
        index++;
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::manage(std::vector< ::fwGui::container::fwMenuItem::sptr > menuItems )
{
    ::fwGui::container::fwMenuItem::sptr menuItem;
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_actionSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < menuItems.size());
        menuItem = menuItems.at( sid.second.first );
        ::fwGui::GuiRegistry::registerActionSIDToParentSID(sid.first, m_sid);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            OSLM_ASSERT("Service "<<sid.first <<" must be stopped.", service->isStopped() );
            service->start();
        }
        else
        {
            bool service_exists = ::fwTools::fwID::exist(sid.first );
            if (!service_exists || ::fwServices::get( sid.first )->isStopped())
            {
                ::fwGui::GuiRegistry::actionServiceStopping(sid.first);
            }
            else
            {
                ::fwGui::GuiRegistry::actionServiceStarting(sid.first);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::manage(std::vector< ::fwGui::container::fwMenu::sptr > menus )
{
    ::fwGui::container::fwMenu::sptr menu;
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_menuSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < menus.size());
        menu = menus.at( sid.second.first );
        ::fwGui::GuiRegistry::registerSIDMenu(sid.first, menu);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            OSLM_ASSERT("Service "<<sid.first <<" must be stopped.", service->isStopped() );
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarRegistrar::manage(std::vector< ::fwGui::container::fwContainer::sptr > containers )
{
    ::fwGui::container::fwContainer::sptr container;
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_editorSids)
    {
        OSLM_ASSERT("Container index "<< sid.second.first <<" is bigger than subViews size!", sid.second.first < containers.size());
        container = containers.at( sid.second.first );
        ::fwGui::GuiRegistry::registerSIDContainer(sid.first, container);
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            OSLM_ASSERT("Service "<<sid.first <<" must be stopped.", service->isStopped() );
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
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterActionSIDToParentSID(sid.first, m_sid);
    }
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_menuSids)
    {
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDMenu(sid.first);
    }
    BOOST_FOREACH( SIDToolBarMapType::value_type sid, m_editorSids)
    {
        if(sid.second.second) //service is auto started?
        {
            OSLM_ASSERT("Service "<<sid.first <<" not exists.", ::fwTools::fwID::exist(sid.first ) );
            ::fwServices::IService::sptr service = ::fwServices::get( sid.first ) ;
            service->stop();
        }
        ::fwGui::GuiRegistry::unregisterSIDContainer(sid.first);
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
