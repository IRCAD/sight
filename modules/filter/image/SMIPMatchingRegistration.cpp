/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "SMIPMatchingRegistration.hpp"

#include <core/tools/Dispatcher.hpp>

#include <filter/image/MIPMatchingRegistration.hpp>
#include <filter/image/Resampler.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

#include <functional>
#include <numeric>

namespace sight::module::filter::image
{

SMIPMatchingRegistration::SMIPMatchingRegistration() noexcept :
    service::IRegisterer()
{
}

//------------------------------------------------------------------------------

SMIPMatchingRegistration::~SMIPMatchingRegistration() noexcept
{
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::starting()
{
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::stopping()
{
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::configuring()
{
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::updating()
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

    core::Type type = moving->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, sight::filter::image::RegistrationDispatch>
    ::invoke(type, params);

    transform->signal<data::Matrix4::ModifiedSignalType>(data::Matrix4::s_MODIFIED_SIG)->asyncEmit();
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::computeRegistration(core::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace sight::module::filter::image
