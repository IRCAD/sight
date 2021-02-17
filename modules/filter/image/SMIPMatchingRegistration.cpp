/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectReadToWriteLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <service/macros.hpp>

#include <filter/image/MIPMatchingRegistration.hpp>
#include <filter/image/Resampler.hpp>

#include <geometry/data/Matrix4.hpp>

#include <functional>
#include <numeric>

namespace sight::modules::filter::image
{

fwServicesRegisterMacro( ::sight::service::IRegisterer,
                         modules::filter::image::SMIPMatchingRegistration)

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
    auto fixed     = this->getInput< data::Image>("fixed");
    auto moving    = this->getInput< data::Image>("moving");
    auto transform = this->getInOut< data::Matrix4>("transform");
    SLM_ASSERT("Missing required input 'fixed'", fixed);
    SLM_ASSERT("Missing required input 'moving'", moving);
    SLM_ASSERT("Missing required inout 'transform'", transform);

    sight::filter::image::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = transform;

    {
        data::mt::ObjectReadLock movingLock(moving);
        data::mt::ObjectReadLock fixedLock(fixed);
        data::mt::ObjectWriteLock transformLock(transform);
        core::tools::Type type = moving->getType();
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, sight::filter::image::RegistrationDispatch >
        ::invoke( type, params );
    }

    transform->signal< data::Matrix4::ModifiedSignalType>(
        data::Matrix4::s_MODIFIED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::computeRegistration(core::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace sight::modules::filter::image
