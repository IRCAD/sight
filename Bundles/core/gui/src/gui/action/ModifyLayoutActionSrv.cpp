/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>
#include <fwGui/IGuiContainerSrv.hpp>

#include "gui/action/ModifyLayoutActionSrv.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::gui::action::ModifyLayoutActionSrv , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

ModifyLayoutActionSrv::ModifyLayoutActionSrv() throw()
{}

//-----------------------------------------------------------------------------

ModifyLayoutActionSrv::~ModifyLayoutActionSrv() throw()
{}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::info(std::ostream &_sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;

    BOOST_FOREACH(MoveSrvVectType::value_type elt, m_moveSrv )
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        OSLM_ASSERT( uid << " doesn't exist", ::fwTools::fwID::exist(uid) );
        ::fwServices::IService::sptr service = ::fwServices::get( uid ) ;
        SLM_ASSERT("service not found", service);
        ::fwGui::IGuiContainerSrv::sptr container = ::fwGui::IGuiContainerSrv::dynamicCast(service);
        SLM_ASSERT("::fwGui::IGuiContainerSrv dynamicCast failed", container);

        container->setParent(wid);
        service->update();
    }

    BOOST_FOREACH(EnableSrvVectType::value_type elt, m_enableSrv )
    {
        std::string uid = elt.first;
        bool isEnable = elt.second;
        OSLM_ASSERT( uid << " doesn't exist", ::fwTools::fwID::exist(uid) );
        ::fwServices::IService::sptr service = ::fwServices::get( uid ) ;
        SLM_ASSERT("service not found", service);
        if(service->isStarted())
        {
//            //todo : add methods to enable/disable IGuiContainerSrv, IMenuSrv, IMenuBarSrv and IToolBarSrv
//            ::fwGui::IGuiContainerSrv::sptr containerSrv = ::fwGui::IGuiContainerSrv::dynamicCast(service);
//            if(containerSrv)
//            {
//                container->getContainer()->setEnable(isEnable);
//            }
            ::fwGui::IActionSrv::sptr actionSrv = ::fwGui::IActionSrv::dynamicCast(service);
            if(actionSrv)
            {
                actionSrv->setIsExecutable(isEnable);
            }
        }
    }

    BOOST_FOREACH(ShowSrvVectType::value_type elt, m_showSrv)
    {
        std::string wid = elt.first;
        ::boost::logic::tribool isVisible = elt.second;
        ::fwGui::container::fwContainer::sptr container = ::fwGui::GuiRegistry::getWIDContainer(wid);
        OSLM_ASSERT("::fwGui::IGuiContainerSrv " << wid << " is unknown", container);

        if(isVisible)
        {
            container->setVisible(true);
        }
        else if(!isVisible)
        {
            container->setVisible(false);
        }
        else
        {
            container->setVisible(this->getIsActive());
        }
    }
}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ModifyLayoutActionSrv::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    if( ! vectConfig.empty() )
    {
        ConfigurationType config = vectConfig.at(0);
        BOOST_FOREACH(ConfigurationType actionCfg, config->getElements() )
        {
            if(actionCfg->getName() == "move")
            {
                SLM_ASSERT("Attribute uid missing", actionCfg->hasAttribute("uid")) ;
                std::string uuid = actionCfg->getExistingAttributeValue("uid") ;
                SLM_ASSERT("Attribute wid missing", actionCfg->hasAttribute("wid")) ;
                std::string wid = actionCfg->getExistingAttributeValue("wid") ;

                m_moveSrv.push_back( std::make_pair(uuid, wid) );
            }
            else if(actionCfg->getName() == "show"
                    || actionCfg->getName() == "hide"
                    || actionCfg->getName() == "show_or_hide")
            {
                SLM_ASSERT("Attribute wid missing", actionCfg->hasAttribute("wid")) ;
                std::string wid = actionCfg->getExistingAttributeValue("wid") ;
                ::boost::logic::tribool isVisible;
                if (actionCfg->getName() == "show")
                {
                    isVisible = true;
                }
                else if (actionCfg->getName() == "hide")
                {
                    isVisible = false;
                }
                else
                {
                    isVisible = ::boost::logic::indeterminate;
                }
                m_showSrv.push_back( std::make_pair(wid, isVisible) );
            }
            else if(actionCfg->getName() == "enable" || actionCfg->getName() == "disable")
            {
                SLM_ASSERT("Attribute uid missing", actionCfg->hasAttribute("uid")) ;
                std::string uuid = actionCfg->getExistingAttributeValue("uid") ;
                bool isEnable = (actionCfg->getName() == "enable");

                m_enableSrv.push_back( std::make_pair(uuid, isEnable) );
            }
            else
            {
                OSLM_FATAL( "Invalid tag name "<<actionCfg->getName());
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace gui

