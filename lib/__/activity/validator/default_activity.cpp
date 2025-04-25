/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "activity/validator/default_activity.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/activity.hpp>
#include <data/map.hpp>
#include <data/vector.hpp>

namespace sight::activity::validator
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::activity::validator::default_activity);

//-----------------------------------------------------------------------------

validator::return_t default_activity::validate(const data::activity::csptr& _activity) const
{
    return validator::activity::check_requirements(_activity);
}

//-----------------------------------------------------------------------------

} // namespace sight::activity::validator
