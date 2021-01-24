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

#include "opItkRegistration/SMIPMatchingRegistration.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Image.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectReadToWriteLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <itkRegistrationOp/MIPMatchingRegistration.hpp>
#include <itkRegistrationOp/Resampler.hpp>

#include <functional>
#include <numeric>

namespace opItkRegistration
{

fwServicesRegisterMacro(::fwServices::IRegisterer,
                        ::opItkRegistration::SMIPMatchingRegistration)

SMIPMatchingRegistration::SMIPMatchingRegistration() noexcept :
    ::fwServices::IRegisterer()
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
    auto transform = this->getInOut< data::TransformationMatrix3D>("transform");
    SLM_ASSERT("Missing required input 'fixed'", fixed);
    SLM_ASSERT("Missing required input 'moving'", moving);
    SLM_ASSERT("Missing required inout 'transform'", transform);

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = transform;

    {
        data::mt::ObjectReadLock movingLock(moving);
        data::mt::ObjectReadLock fixedLock(fixed);
        data::mt::ObjectWriteLock transformLock(transform);
        core::tools::Type type = moving->getType();
        core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, ::itkRegistrationOp::RegistrationDispatch >
        ::invoke( type, params );
    }

    transform->signal< data::TransformationMatrix3D::ModifiedSignalType>(
        data::TransformationMatrix3D::s_MODIFIED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::computeRegistration(core::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace opItkRegistration
