/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <core/tools/dispatcher.hpp>

#include <filter/image/mip_matching_registration.hpp>
#include <filter/image/resampler.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <functional>
#include <numeric>

namespace sight::module::filter::image
{

mip_matching_registration::mip_matching_registration() noexcept =
    default;

//------------------------------------------------------------------------------

mip_matching_registration::~mip_matching_registration() noexcept =
    default;

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

    sight::filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed.get_shared();
    params.moving    = moving.get_shared();
    params.transform = transform.get_shared();

    core::type type = moving->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, sight::filter::image::RegistrationDispatch>
    ::invoke(type, params);

    transform->signal<data::matrix4::modified_signal_t>(data::matrix4::MODIFIED_SIG)->async_emit();
}

//------------------------------------------------------------------------------

void mip_matching_registration::computeRegistration(core::hires_clock::type /*timestamp*/)
{
    this->updating();
}

} // namespace sight::module::filter::image
