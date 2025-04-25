/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "service/op.hpp"

#include "service/base.hpp"
#include "service/extension/factory.hpp"
#include "service/macros.hpp"

#include <vector>

namespace sight::service
{

//------------------------------------------------------------------------------

service::base::sptr add(const std::string& _impl_type, const std::string& _uid)
{
    service::base::sptr srv;
    srv = service::extension::factory::get()->create(_impl_type);
    service::register_service(srv);
    SIGHT_THROW_IF("Failed to add " + _impl_type, !srv);
    if(!_uid.empty())
    {
        SIGHT_ASSERT("Try to set ID: " + _uid + " but already has an ID: " + srv->get_id(), !srv->has_id());
        srv->set_id(_uid);
    }

    return srv;
}

//------------------------------------------------------------------------------

service::base::sptr get(std::string _uid)
{
    SIGHT_ASSERT("service does not exist with uid " << _uid, core::id::exist(_uid));
    return std::dynamic_pointer_cast<service::base>(core::id::get_object(_uid));
}

//------------------------------------------------------------------------------

} // namespace sight::service
