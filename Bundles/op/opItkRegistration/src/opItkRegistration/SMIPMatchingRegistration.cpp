/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SMIPMatchingRegistration.hpp"

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <itkRegistrationOp/MIPMatchingRegistration.hpp>
#include <itkRegistrationOp/Resampler.hpp>

#include <functional>
#include <numeric>

namespace opItkRegistration
{

fwServicesRegisterMacro(::fwServices::IRegisterer,
                        ::opItkRegistration::SMIPMatchingRegistration);

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
    auto fixed     = this->getInput< ::fwData::Image>("fixed");
    auto moving    = this->getInput< ::fwData::Image>("moving");
    auto transform = this->getInOut< ::fwData::TransformationMatrix3D>("transform");
    SLM_ASSERT("Missing required input 'fixed'", fixed);
    SLM_ASSERT("Missing required input 'moving'", moving);
    SLM_ASSERT("Missing required inout 'transform'", transform);

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.fixed     = fixed;
    params.moving    = moving;
    params.transform = transform;

    {
        ::fwData::mt::ObjectReadLock movingLock(moving);
        ::fwData::mt::ObjectReadLock fixedLock(fixed);
        ::fwData::mt::ObjectWriteLock transformLock(transform);
        ::fwTools::DynamicType type = moving->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ::itkRegistrationOp::RegistrationDispatch >
        ::invoke( type, params );
    }

    transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType>(
        ::fwData::TransformationMatrix3D::s_MODIFIED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SMIPMatchingRegistration::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace opItkRegistration
