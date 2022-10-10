/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

namespace sight::module::ui::base
{

//-----------------------------------------------------------------------------

SModifyLayout::SModifyLayout() noexcept =
    default;

//-----------------------------------------------------------------------------

SModifyLayout::~SModifyLayout() noexcept =
    default;

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

void SModifyLayout::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SModifyLayout::updating()
{
    for(const MoveSrvVectType::value_type& elt : m_moveSrv)
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::fwID::exist(uid));
        service::IService::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        sight::ui::base::IGuiContainer::sptr container = sight::ui::base::IGuiContainer::dynamicCast(service);
        SIGHT_ASSERT("::ui::base::IGuiContainer dynamicCast failed", container);

        container->setParent(wid);
        service->update();
    }

    for(const EnableSrvVectType::value_type& elt : m_enableSrv)
    {
        std::string uid = elt.first;
        bool isEnable   = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::fwID::exist(uid));
        service::IService::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        if(service->isStarted())
        {
            sight::ui::base::IGuiContainer::sptr containerSrv = sight::ui::base::IGuiContainer::dynamicCast(
                service
            );
            if(containerSrv)
            {
                containerSrv->getContainer()->setEnabled(isEnable);
            }

            auto actionSrv = sight::ui::base::IAction::dynamicCast(service);
            if(actionSrv)
            {
                actionSrv->setEnabled(isEnable);
            }
        }
    }

    for(const ShowSrvVectType::value_type& elt : m_showSrvWid)
    {
        std::string wid                                         = elt.first;
        boost::logic::tribool isVisible                         = elt.second;
        sight::ui::base::container::fwContainer::sptr container =
            sight::ui::base::GuiRegistry::getWIDContainer(wid);
        SIGHT_ASSERT("::ui::base::IGuiContainer " << wid << " is unknown", container);

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
            container->setVisible(this->checked());
        }
    }

    for(const ShowSrvVectType::value_type& elt : m_showSrvSid)
    {
        std::string uid                 = elt.first;
        boost::logic::tribool isVisible = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::fwID::exist(uid));
        service::IService::sptr service = service::get(uid);

        auto containerSrv = sight::ui::base::IGuiContainer::dynamicCast(service);
        SIGHT_ASSERT("::ui::base::IGuiContainer dynamicCast failed", containerSrv);

        sight::ui::base::container::fwContainer::sptr container = containerSrv->getContainer();

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
            container->setVisible(this->checked());
        }
    }
}

//-----------------------------------------------------------------------------

void SModifyLayout::configuring()
{
    this->initialize();

    const auto& config = this->getConfigTree();

    for(const auto& actionCfg : config.get_child("config"))
    {
        if(actionCfg.first == "move")
        {
            const auto uid = actionCfg.second.get<std::string>("<xmlattr>.uid");
            const auto wid = actionCfg.second.get<std::string>("<xmlattr>.wid");

            m_moveSrv.push_back(std::make_pair(uid, wid));
        }
        else if(actionCfg.first == "show"
                || actionCfg.first == "hide"
                || actionCfg.first == "show_or_hide")
        {
            boost::logic::tribool isVisible;
            if(actionCfg.first == "show")
            {
                isVisible = true;
            }
            else if(actionCfg.first == "hide")
            {
                isVisible = false;
            }
            else
            {
                isVisible = boost::logic::indeterminate;
            }

            if(const auto wid = actionCfg.second.get_optional<std::string>("<xmlattr>.wid"); wid.has_value())
            {
                m_showSrvWid.push_back(std::make_pair(wid.value(), isVisible));
            }
            else if(const auto sid = actionCfg.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
            {
                m_showSrvSid.push_back(std::make_pair(sid.value(), isVisible));
            }
            else
            {
                SIGHT_ERROR("Attribute wid or sid missing");
            }
        }
        else if(actionCfg.first == "enable" || actionCfg.first == "disable")
        {
            const auto uid = actionCfg.second.get<std::string>("<xmlattr>.uid");
            bool isEnable  = (actionCfg.first == "enable");

            m_enableSrv.push_back(std::make_pair(uid, isEnable));
        }
        else
        {
            SIGHT_FATAL("Invalid tag name " << actionCfg.first);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base
