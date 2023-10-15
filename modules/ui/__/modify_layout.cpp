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

#include "modify_layout.hpp"

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>
#include <ui/__/service.hpp>

namespace sight::module::ui
{

//-----------------------------------------------------------------------------

void modify_layout::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    for(const auto& actionCfg : config.get_child("config"))
    {
        if(actionCfg.first == "move")
        {
            const auto uid = actionCfg.second.get<std::string>("<xmlattr>.uid");
            const auto wid = actionCfg.second.get<std::string>("<xmlattr>.wid");

            m_moveSrv.emplace_back(uid, wid);
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
                m_showSrvWid.emplace_back(wid.value(), visibility);
            }
            else if(const auto sid = actionCfg.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
            {
                m_showSrvSid.emplace_back(sid.value(), visibility);
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

            m_enableSrv.emplace_back(uid, isEnable);
        }
        else
        {
            SIGHT_FATAL("Invalid tag name " << actionCfg.first);
        }
    }
}

//-----------------------------------------------------------------------------

void modify_layout::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void modify_layout::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void modify_layout::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void modify_layout::updating()
{
    for(const MoveSrvVectType::value_type& elt : m_moveSrv)
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::id::exist(uid));
        service::base::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        auto container = std::dynamic_pointer_cast<sight::ui::service>(service);
        SIGHT_ASSERT("::ui::container dynamicCast failed", container);

        container->setParent(wid);
        service->update();
    }

    for(const EnableSrvVectType::value_type& elt : m_enableSrv)
    {
        std::string uid = elt.first;
        bool isEnable   = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::id::exist(uid));
        service::base::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        if(service->started())
        {
            auto containerSrv = std::dynamic_pointer_cast<sight::ui::service>(service);
            if(containerSrv)
            {
                containerSrv->getContainer()->setEnabled(isEnable);
            }

            auto actionSrv = std::dynamic_pointer_cast<sight::ui::action>(service);
            if(actionSrv)
            {
                actionSrv->setEnabled(isEnable);
            }
        }
    }

    for(const ShowSrvVectType::value_type& elt : m_showSrvWid)
    {
        const std::string wid                        = elt.first;
        const auto visibility                        = elt.second;
        sight::ui::container::widget::sptr container =
            sight::ui::registry::getWIDContainer(wid);
        SIGHT_ASSERT("::ui::container " << wid << " is unknown", container);

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
        SIGHT_ASSERT(uid << " doesn't exist", core::tools::id::exist(uid));
        service::base::sptr service = service::get(uid);

        auto containerSrv = std::dynamic_pointer_cast<sight::ui::service>(service);
        SIGHT_ASSERT("::ui::container dynamicCast failed", containerSrv);

        sight::ui::container::widget::sptr container = containerSrv->getContainer();

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

} // namespace sight::module::ui
