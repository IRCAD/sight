/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "filter/image/mip_matching_registration.hpp"

#include "filter/image/detail/mip_matching_registration.hxx"

//-----------------------------------------------------------------------------

namespace sight::filter::image
{

//------------------------------------------------------------------------------

void mip_matching_register(const data::image& _fixed, const data::image& _moving, data::matrix4& _transform)
{
    sight::filter::image::registration_dispatch::parameters params
    {
        .fixed     = _fixed.get_const_sptr(),
        .moving    = _moving.get_const_sptr(),
        .transform = _transform.get_sptr()
    };

    core::type type = _moving.type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, sight::filter::image::registration_dispatch>
    ::invoke(type, params);
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
