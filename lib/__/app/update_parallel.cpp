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

#include "update_parallel.hpp"

#include "detail/update_registry.hpp"

#include <boost/thread/futures/wait_for_all.hpp>

namespace sight::app
{

//------------------------------------------------------------------------------

void update_parallel::starting()
{
    this->reset_stop_request();

    if(!this->parent().empty())
    {
        app::register_updater(this->parent(), this->get_sptr());
    }
}

//-----------------------------------------------------------------------------

void update_parallel::stopping()
{
    if(!this->parent().empty())
    {
        app::unregister_updater(this->parent());
    }
}

//-----------------------------------------------------------------------------

void update_parallel::updating()
{
    std::vector<std::pair<sight::service::base::sptr, std::string> > services;
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
            if(srv->started())
            {
                services.emplace_back(srv, element.slot);
                if(srv->is_auto_connected())
                {
                    SIGHT_ERROR(
                        "[sight::app::update_parallel] Service " << std::quoted(element.uid)
                        << " is auto-connected, this is likely to introduce timing issues."
                    );
                }
            }
            else
            {
                SIGHT_WARN(
                    "[sight::app::update_parallel] Service " << std::quoted(element.uid)
                    << " is not started, skipping update."
                );
                services.clear();
                break;
            }
        }
        else
        {
            SIGHT_WARN(
                "[sight::app::update_parallel] Service " << std::quoted(element.uid)
                << " does not exists, skipping update."
            );
            services.clear();
            break;
        }
    }

    std::vector<service::base::shared_future_t> futures;
    std::ranges::for_each(services, [&futures](const auto& _x){futures.push_back(_x.first->update());});
    boost::wait_for_all(futures.begin(), futures.end());
}

//-----------------------------------------------------------------------------

} // namespace sight::app
