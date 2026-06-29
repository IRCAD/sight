/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "mip_matching_registration.hpp"

#include <filter/image/mip_matching_registration.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

void mip_matching_registration::starting()
{
}

//------------------------------------------------------------------------------

void mip_matching_registration::stopping()
{
}

//------------------------------------------------------------------------------

void mip_matching_registration::configuring()
{
}

//------------------------------------------------------------------------------

void mip_matching_registration::updating()
{
    const auto fixed  = m_target.lock();
    const auto moving = m_source.lock();
    auto transform    = m_transform.lock();

    SIGHT_ASSERT("Missing required input 'fixed'", fixed);
    SIGHT_ASSERT("Missing required input 'moving'", moving);
    SIGHT_ASSERT("Missing required inout 'transform'", transform);

    sight::filter::image::mip_matching_register(*fixed, *moving, *transform);

    transform->async_emit(data::signals::MODIFIED);
}

//------------------------------------------------------------------------------

void mip_matching_registration::compute_registration(core::clock::type /*timestamp*/)
{
    this->updating();
}

} // namespace sight::module::filter::image
