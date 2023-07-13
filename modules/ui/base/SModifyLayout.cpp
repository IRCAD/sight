/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <service/op/Get.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/GuiRegistry.hpp>
#include <ui/base/IGuiContainer.hpp>

namespace sight::module::ui::base
{

//-----------------------------------------------------------------------------

void SModifyLayout::configuring()
{
    this->initialize();

    const auto& config = this->getConfiguration();

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
                || actionCfg.first == "show_or_hide"
                || actionCfg.first == "toggle")
        {
            visibility_t visibility = visibility_t::SHOW;
            if(actionCfg.first == "show")
            {
                visibility = visibility_t::SHOW;
            }
            else if(actionCfg.first == "hide")
            {
                visibility = visibility_t::HIDE;
            }
            else if(actionCfg.first == "show_or_hide")
            {
                visibility = visibility_t::SHOW_OR_HIDE;
            }
            else if(actionCfg.first == "toggle")
            {
                visibility = visibility_t::TOGGLE;
            }
            else
            {
                SIGHT_FATAL("Unknown visiblity parameter value : " + actionCfg.first)
            }

            if(const auto wid = actionCfg.second.get_optional<std::string>("<xmlattr>.wid"); wid.has_value())
            {
                m_showSrvWid.push_back(std::make_pair(wid.value(), visibility));
            }
            else if(const auto sid = actionCfg.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
            {
                m_showSrvSid.push_back(std::make_pair(sid.value(), visibility));
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
        const std::string wid                                   = elt.first;
        const auto visibility                                   = elt.second;
        sight::ui::base::container::fwContainer::sptr container =
            sight::ui::base::GuiRegistry::getWIDContainer(wid);
        SIGHT_ASSERT("::ui::base::IGuiContainer " << wid << " is unknown", container);

        if(visibility == visibility_t::SHOW)
        {
            container->setVisible(true);
        }
        else if(visibility == visibility_t::HIDE)
        {
            container->setVisible(false);
        }
        else if(visibility == visibility_t::SHOW_OR_HIDE)
        {
            container->setVisible(this->checked());
        }
        else if(visibility == visibility_t::TOGGLE)
        {
            container->setVisible(!container->isShownOnScreen());
        }
        else
        {
            SIGHT_FATAL("Unknown visiblity parameter value");
        }
    }

    for(const ShowSrvVectType::value_type& elt : m_showSrvSid)
    {
        std::string uid       = elt.first;
        const auto visibility = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::fwID::exist(uid));
        service::IService::sptr service = service::get(uid);

        auto containerSrv = sight::ui::base::IGuiContainer::dynamicCast(service);
        SIGHT_ASSERT("::ui::base::IGuiContainer dynamicCast failed", containerSrv);

        sight::ui::base::container::fwContainer::sptr container = containerSrv->getContainer();

        if(visibility == visibility_t::SHOW)
        {
            container->setVisible(true);
        }
        else if(visibility == visibility_t::HIDE)
        {
            container->setVisible(false);
        }
        else if(visibility == visibility_t::SHOW_OR_HIDE)
        {
            container->setVisible(this->checked());
        }
        else if(visibility == visibility_t::TOGGLE)
        {
            container->setVisible(!container->isShownOnScreen());
        }
        else
        {
            SIGHT_FATAL("Unknown visiblity parameter value");
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base
