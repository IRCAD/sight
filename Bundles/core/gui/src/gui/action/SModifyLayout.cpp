/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SModifyLayout.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>
#include <fwGui/IGuiContainerSrv.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

namespace gui
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SModifyLayout, ::fwData::Object );

//-----------------------------------------------------------------------------

SModifyLayout::SModifyLayout() throw()
{
}

//-----------------------------------------------------------------------------

SModifyLayout::~SModifyLayout() throw()
{
}

//-----------------------------------------------------------------------------

void SModifyLayout::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SModifyLayout::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SModifyLayout::info(std::ostream &_sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SModifyLayout::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    for(MoveSrvVectType::value_type elt :  m_moveSrv )
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        OSLM_ASSERT( uid << " doesn't exist", ::fwTools::fwID::exist(uid) );
        ::fwServices::IService::sptr service = ::fwServices::get( uid );
        SLM_ASSERT("service not found", service);
        ::fwGui::IGuiContainerSrv::sptr container = ::fwGui::IGuiContainerSrv::dynamicCast(service);
        SLM_ASSERT("::fwGui::IGuiContainerSrv dynamicCast failed", container);

        container->setParent(wid);
        service->update();
    }

    for(EnableSrvVectType::value_type elt :  m_enableSrv )
    {
        std::string uid = elt.first;
        bool isEnable   = elt.second;
        OSLM_ASSERT( uid << " doesn't exist", ::fwTools::fwID::exist(uid) );
        ::fwServices::IService::sptr service = ::fwServices::get( uid );
        SLM_ASSERT("service not found", service);
        if(service->isStarted())
        {
            ::fwGui::IGuiContainerSrv::sptr containerSrv = ::fwGui::IGuiContainerSrv::dynamicCast(service);
            if(containerSrv)
            {
                containerSrv->getContainer()->setEnabled(isEnable);
            }
            ::fwGui::IActionSrv::sptr actionSrv = ::fwGui::IActionSrv::dynamicCast(service);
            if(actionSrv)
            {
                actionSrv->setIsExecutable(isEnable);
            }
        }
    }

    for(ShowSrvVectType::value_type elt :  m_showSrvWid)
    {
        std::string wid = elt.first;
        ::boost::logic::tribool isVisible               = elt.second;
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

    for(ShowSrvVectType::value_type elt :  m_showSrvSid)
    {
        std::string uid = elt.first;
        ::boost::logic::tribool isVisible = elt.second;
        OSLM_ASSERT( uid << " doesn't exist", ::fwTools::fwID::exist(uid) );
        ::fwServices::IService::sptr service = ::fwServices::get( uid );

        ::fwGui::IGuiContainerSrv::sptr containerSrv = ::fwGui::IGuiContainerSrv::dynamicCast(service);
        SLM_ASSERT("::fwGui::IGuiContainerSrv dynamicCast failed", containerSrv);

        ::fwGui::container::fwContainer::sptr container = containerSrv->getContainer();

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

void SModifyLayout::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    if( !vectConfig.empty() )
    {
        ConfigurationType config = vectConfig.at(0);
        for(ConfigurationType actionCfg :  config->getElements() )
        {
            if(actionCfg->getName() == "move")
            {
                SLM_ASSERT("Attribute uid missing", actionCfg->hasAttribute("uid"));
                std::string uuid = actionCfg->getExistingAttributeValue("uid");
                SLM_ASSERT("Attribute wid missing", actionCfg->hasAttribute("wid"));
                std::string wid = actionCfg->getExistingAttributeValue("wid");

                m_moveSrv.push_back( std::make_pair(uuid, wid) );
            }
            else if(actionCfg->getName() == "show"
                    || actionCfg->getName() == "hide"
                    || actionCfg->getName() == "show_or_hide")
            {
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

                if(actionCfg->hasAttribute("wid"))
                {
                    std::string wid = actionCfg->getExistingAttributeValue("wid");
                    m_showSrvWid.push_back( std::make_pair(wid, isVisible) );
                }
                else if(actionCfg->hasAttribute("sid"))
                {
                    std::string sid = actionCfg->getExistingAttributeValue("sid");
                    m_showSrvSid.push_back( std::make_pair(sid, isVisible) );
                }
                else
                {
                    SLM_ERROR("Attribute wid or sid missing");
                }
            }
            else if(actionCfg->getName() == "enable" || actionCfg->getName() == "disable")
            {
                SLM_ASSERT("Attribute uid missing", actionCfg->hasAttribute("uid"));
                std::string uuid = actionCfg->getExistingAttributeValue("uid");
                bool isEnable    = (actionCfg->getName() == "enable");

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

