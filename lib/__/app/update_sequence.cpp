/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include "update_sequence.hpp"

#include "detail/update_registry.hpp"

namespace sight::app
{

//------------------------------------------------------------------------------

void update_sequence::starting()
{
    this->reset_stop_request();

    if(!this->parent().empty())
    {
        app::register_updater(this->parent(), this->get_sptr());
    }

    if(this->loop())
    {
        this->updating();
    }
}

//-----------------------------------------------------------------------------

void update_sequence::stopping()
{
    if(!this->parent().empty())
    {
        app::unregister_updater(this->parent());
    }
}

//-----------------------------------------------------------------------------

void update_sequence::updating()
{
    std::vector<std::pair<sight::service::base::sptr, std::string> > services;

    std::set<std::string> is_going_to_be_started;
    std::set<std::string> is_going_to_be_stopped;

    for(const auto& element : this->elements())
    {
        sight::service::base::sptr srv;
        if(element.type == type_t::service)
        {
            srv = std::dynamic_pointer_cast<sight::service::base>(sight::core::id::get_object(element.uid));
        }
        else
        {
            srv = app::get_updater(element.uid);
        }

        if(srv != nullptr)
        {
            const auto is_slot_start = (element.slot == service::base::slots::START);
            const auto is_slot_stop  = (element.slot == service::base::slots::STOP);

            if(is_slot_start)
            {
                // memorize that a start of this service is called.
                is_going_to_be_started.insert(element.uid);
            }
            else if(is_slot_stop)
            {
                //memorize that a stop of this service is called.
                is_going_to_be_stopped.insert(element.uid);
            }

            const auto is_started      = srv->started();
            const auto will_be_started = is_going_to_be_started.contains(element.uid);
            const auto is_stopped      = srv->stopped();
            const auto will_be_stopped = is_going_to_be_stopped.contains(element.uid);

            // Service is started, or will be started and current slot is not start = OK we add the slot.
            if((is_started or will_be_started) and not is_slot_start)
            {
                services.emplace_back(srv, element.slot);
                if(srv->is_auto_connected())
                {
                    SIGHT_ERROR(
                        "[sight::app::update_sequence] Service " << std::quoted(element.uid)
                        << " is auto-connected, this is likely to introduce timing issues."
                    );
                }
            }
            // Service is stopped or will be stopped and current slot isn't start or stop = NOK we discard the sequence
            else if((is_stopped or will_be_stopped) and not is_slot_start and not is_slot_stop)
            {
                if(not element.ignore_stopped)
                {
                    SIGHT_WARN(
                        "[sight::app::update_sequence] Service " << std::quoted(element.uid)
                        << " is stopped or will be stopped."
                    );
                    services.clear();
                    break;
                }
            }
            // Service isn't stopped and slot isn't stop = OK we add slot
            else if(not (is_stopped or will_be_stopped) or not is_slot_stop)
            {
                services.emplace_back(srv, element.slot);
                if(srv->is_auto_connected())
                {
                    SIGHT_ERROR(
                        "[sight::app::update_sequence] Service " << std::quoted(element.uid)
                        << " is auto-connected, this is likely to introduce timing issues."
                    );
                }
            }
        }
    }

    using namespace std::chrono_literals;

    for(const auto& srv : services)
    {
        if(this->stop_requested())
        {
            return;
        }

        if(this->worker() == srv.first->worker())
        {
            srv.first->slot(srv.second)->run();
        }
        else
        {
            const auto future = srv.first->slot(srv.second)->async_run();
        }
    }

    if(this->loop() && !this->stop_requested())
    {
        this->slot(service::base::slots::UPDATE)->async_run();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::app
