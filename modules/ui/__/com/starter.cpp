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
    this->action_service_starting();
}

//-----------------------------------------------------------------------------

void starter::stopping()
{
    std::vector<service::base::shared_future_t> futures;

    for(const auto& service_uid : boost::adaptors::reverse(m_uuid_services))
    {
        bool srv_exists = core::tools::id::exist(service_uid.first);
        if(srv_exists && (m_id_started_srv_set.find(service_uid.first) != m_id_started_srv_set.end()))
        {
            service::base::sptr service = service::get(service_uid.first);
            if(service->started())
            {
                futures.push_back(service->stop());
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

    this->action_service_stopping();
}

//-----------------------------------------------------------------------------

void starter::info(std::ostream& _sstream)
{
    _sstream << "Starter action" << std::endl;
}

//-----------------------------------------------------------------------------

void starter::updating()
{
    for(auto& m_uuid_service : m_uuid_services)
    {
        action_t action = m_uuid_service.second;
        id_srv_t uid    = m_uuid_service.first;
        bool srv_exists = core::tools::id::exist(uid);

        // Manage special action
        if(action == start_if_exists)
        {
            if(srv_exists)
            {
                action = start;
            }
            else
            {
                action = do_nothing;
            }
        }
        else if(action == stop_if_exists)
        {
            if(srv_exists)
            {
                action = stop;
            }
            else
            {
                action = do_nothing;
            }
        }

        if(action != do_nothing)
        {
            sight::ui::lock_action lock(this->get_sptr());

            service::base::sptr service = service::get(uid);
            SIGHT_ASSERT("service not found", service);
            switch(action)
            {
                case start_or_stop:
                    if(service->stopped())
                    {
                        service->start();
                        service->update();
                        m_id_started_srv_set.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_id_started_srv_set.erase(uid);
                    }

                    break;

                case start_only_or_stop:
                    if(service->stopped())
                    {
                        service->start();
                        m_id_started_srv_set.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_id_started_srv_set.erase(uid);
                    }

                    break;

                case start:
                    if(service->stopped())
                    {
                        service->start();
                        m_id_started_srv_set.insert(uid);
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->get_id() << " is not stopped");
                    }

                    service->update();
                    break;

                case stop:
                    if(service->started())
                    {
                        service->stop();
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->get_id() << " is not started");
                    }

                    break;

                case start_only:
                    if(service->stopped())
                    {
                        service->start();
                        m_id_started_srv_set.insert(uid);
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
                sight::ui::dialog::message::warning
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
        action_t action {action_t::do_nothing};
        if(action_type == "start")
        {
            action = start;
        }
        else if(action_type == "stop")
        {
            action = stop;
        }
        else if(action_type == "start_or_stop")
        {
            action = start_or_stop;
        }
        else if(action_type == "start_only_or_stop")
        {
            action = start_only_or_stop;
        }
        else if(action_type == "start_if_exists")
        {
            action = start_if_exists;
        }
        else if(action_type == "stop_if_exists")
        {
            action = stop_if_exists;
        }
        else if(action_type == "start_only")
        {
            action = start_only;
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

        m_uuid_services.emplace_back(uuid, action);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::com
