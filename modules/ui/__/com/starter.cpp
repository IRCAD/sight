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

#include "starter.hpp"

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>

#include <boost/range/adaptor/reversed.hpp>

namespace sight::module::ui::com
{

//-----------------------------------------------------------------------------

starter::starter() noexcept
{
    SIGHT_WARN(
        "'sight::module::ui::com::starter' is deprecated and will be removed in Sight 24.0,"
        " start and stop connections are now supported directly in XML."
    );
}

//-----------------------------------------------------------------------------

starter::~starter() noexcept =
    default;

//-----------------------------------------------------------------------------

void starter::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void starter::stopping()
{
    std::vector<service::base::shared_future_t> futures;

    for(const VectPairIDActionType::value_type& service_uid : boost::adaptors::reverse(m_uuidServices))
    {
        bool srv_exists = core::tools::id::exist(service_uid.first);
        if(srv_exists && (m_idStartedSrvSet.find(service_uid.first) != m_idStartedSrvSet.end()))
        {
            service::base::sptr service = service::get(service_uid.first);
            if(service->started())
            {
                futures.push_back(service->stop());
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void starter::info(std::ostream& _sstream)
{
    _sstream << "Starter action" << std::endl;
}

//-----------------------------------------------------------------------------

void starter::updating()
{
    for(auto& m_uuid_service : m_uuidServices)
    {
        action_t action = m_uuid_service.second;
        IDSrvType uid   = m_uuid_service.first;
        bool srv_exists = core::tools::id::exist(uid);

        // Manage special action
        if(action == START_IF_EXISTS)
        {
            if(srv_exists)
            {
                action = START;
            }
            else
            {
                action = DO_NOTHING;
            }
        }
        else if(action == STOP_IF_EXISTS)
        {
            if(srv_exists)
            {
                action = STOP;
            }
            else
            {
                action = DO_NOTHING;
            }
        }

        if(action != DO_NOTHING)
        {
            sight::ui::LockAction lock(this->get_sptr());

            service::base::sptr service = service::get(uid);
            SIGHT_ASSERT("service not found", service);
            switch(action)
            {
                case START_OR_STOP:
                    if(service->stopped())
                    {
                        service->start();
                        service->update();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_idStartedSrvSet.erase(uid);
                    }

                    break;

                case START_ONLY_OR_STOP:
                    if(service->stopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_idStartedSrvSet.erase(uid);
                    }

                    break;

                case START:
                    if(service->stopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->get_id() << " is not stopped");
                    }

                    service->update();
                    break;

                case STOP:
                    if(service->started())
                    {
                        service->stop();
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->get_id() << " is not started");
                    }

                    break;

                case START_ONLY:
                    if(service->stopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->get_id() << " is not stopped");
                    }

                    break;

                default:
                    SIGHT_FATAL(
                        "There is no action (" << action
                        << ") type corresponding to the action id requested for " << uid
                        << "."
                    );
                    break;
            }
        }
        else
        {
            sight::ui::dialog::message::show(
                "Service unavailable",
                "The service is unavailable.",
                sight::ui::dialog::message::WARNING
            );

            SIGHT_INFO("Do nothing for Service " << m_uuid_service.first);
        }
    }
}

//-----------------------------------------------------------------------------

void starter::configuring()
{
    this->initialize();

    for(const auto& action_cfg : this->get_config())
    {
        std::string action_type = action_cfg.first;
        action_t action {action_t::DO_NOTHING};
        if(action_type == "start")
        {
            action = START;
        }
        else if(action_type == "stop")
        {
            action = STOP;
        }
        else if(action_type == "start_or_stop")
        {
            action = START_OR_STOP;
        }
        else if(action_type == "start_only_or_stop")
        {
            action = START_ONLY_OR_STOP;
        }
        else if(action_type == "start_if_exists")
        {
            action = START_IF_EXISTS;
        }
        else if(action_type == "stop_if_exists")
        {
            action = STOP_IF_EXISTS;
        }
        else if(action_type == "start_only")
        {
            action = START_ONLY;
        }
        else if(action_type == "state" || action_type == "<xmlattr>")
        {
            // Ignore
            continue;
        }
        else
        {
            SIGHT_WARN("The \"actionType\":" << action_type << " is not managed by starter");
            continue;
        }

        SIGHT_INFO("starter " << action_cfg.first);
        const auto uuid = action_cfg.second.get<std::string>("<xmlattr>.uid");

        m_uuidServices.emplace_back(uuid, action);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::com
