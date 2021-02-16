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

#include "SModifyLayout.hpp"

#include <core/base.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/GuiRegistry.hpp>
#include <ui/base/IGuiContainer.hpp>

namespace sight::modules::ui::base
{

fwServicesRegisterMacro( ::sight::ui::base::IAction, ::sight::modules::ui::base::SModifyLayout )

//-----------------------------------------------------------------------------

SModifyLayout::SModifyLayout() noexcept
{
}

//-----------------------------------------------------------------------------

SModifyLayout::~SModifyLayout() noexcept
{
}

//-----------------------------------------------------------------------------

void SModifyLayout::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SModifyLayout::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SModifyLayout::info(std::ostream& _sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SModifyLayout::updating()
{

    for(MoveSrvVectType::value_type elt :  m_moveSrv )
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        SLM_ASSERT( uid << " doesn't exist", core::tools::fwID::exist(uid) );
        service::IService::sptr service = service::get( uid );
        SLM_ASSERT("service not found", service);
        ::sight::ui::base::IGuiContainer::sptr container = ::sight::ui::base::IGuiContainer::dynamicCast(service);
        SLM_ASSERT("::ui::base::IGuiContainer dynamicCast failed", container);

        container->setParent(wid);
        service->update();
    }

    for(EnableSrvVectType::value_type elt :  m_enableSrv )
    {
        std::string uid = elt.first;
        bool isEnable   = elt.second;
        SLM_ASSERT( uid << " doesn't exist", core::tools::fwID::exist(uid) );
        service::IService::sptr service = service::get( uid );
        SLM_ASSERT("service not found", service);
        if(service->isStarted())
        {
            ::sight::ui::base::IGuiContainer::sptr containerSrv = ::sight::ui::base::IGuiContainer::dynamicCast(
                service);
            if(containerSrv)
            {
                containerSrv->getContainer()->setEnabled(isEnable);
            }
            auto actionSrv = ::sight::ui::base::IAction::dynamicCast(service);
            if(actionSrv)
            {
                actionSrv->setIsExecutable(isEnable);
            }
        }
    }

    for(ShowSrvVectType::value_type elt :  m_showSrvWid)
    {
        std::string wid = elt.first;
        ::boost::logic::tribool isVisible                         = elt.second;
        ::sight::ui::base::container::fwContainer::sptr container =
            ::sight::ui::base::GuiRegistry::getWIDContainer(wid);
        SLM_ASSERT("::ui::base::IGuiContainer " << wid << " is unknown", container);

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
        SLM_ASSERT( uid << " doesn't exist", core::tools::fwID::exist(uid) );
        service::IService::sptr service = service::get( uid );

        auto containerSrv = ::sight::ui::base::IGuiContainer::dynamicCast(service);
        SLM_ASSERT("::ui::base::IGuiContainer dynamicCast failed", containerSrv);

        ::sight::ui::base::container::fwContainer::sptr container = containerSrv->getContainer();

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

void SModifyLayout::configuring()
{
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
                SLM_FATAL( "Invalid tag name "<<actionCfg->getName());
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::ui::base
