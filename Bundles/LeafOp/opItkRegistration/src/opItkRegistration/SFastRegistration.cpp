/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SFastRegistration.hpp"

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <itkRegistrationOp/FastRegistration.hpp>
#include <itkRegistrationOp/Resampler.hpp>

namespace opItkRegistration
{

fwServicesRegisterMacro(::arServices::IRegisterer,
                        ::opItkRegistration::SFastRegistration);

SFastRegistration::SFastRegistration() noexcept :
    ::arServices::IRegisterer()
{
}

//------------------------------------------------------------------------------

SFastRegistration::~SFastRegistration() noexcept
{
}

//------------------------------------------------------------------------------

void SFastRegistration::starting()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::stopping()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::configuring()
{
}

//------------------------------------------------------------------------------

void SFastRegistration::updating()
{
    auto source        = this->getInput< ::fwData::Image>("source");
    auto target        = this->getInput< ::fwData::Image>("target");
    auto flipTransform = this->getInput< ::fwData::TransformationMatrix3D>("flipTransform");
    auto transform     = this->getInOut< ::fwData::TransformationMatrix3D>("transform");
    SLM_ASSERT("Missing required input 'source'", source);
    SLM_ASSERT("Missing required input 'target'", target);
    SLM_ASSERT("Missing required inout 'transform'", transform);
    SLM_ASSERT("Missing required inout 'flipTransform'", flipTransform);

    std::array<bool, 3> flipAxes;
    for(std::uint8_t i = 0; i != 3; ++i)
    {
        flipAxes[i] = flipTransform->getCoefficient(i, i) < 0.;
    }

    ::itkRegistrationOp::RegistrationDispatch::Parameters params;
    params.source   = source;
    params.target   = target;
    params.flipAxes = flipAxes;
    {
        ::fwData::mt::ObjectReadLock targetLock(target);
        ::fwData::mt::ObjectReadLock srcLock(source);
        ::fwTools::DynamicType type = target->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, ::itkRegistrationOp::RegistrationDispatch >
        ::invoke( type, params );
    }

    OSLM_INFO("Computed fast registration: [" << params.transform[0] << ", "
                                              << params.transform[1] << ", "
                                              << params.transform[2] << "]");

    ::fwData::mt::ObjectWriteLock trfLock(transform);
    ::fwDataTools::TransformationMatrix3D::identity(transform);
    for(std::uint8_t i = 0; i != 3; ++i)
    {
        transform->setCoefficient(i, 3, params.transform[i]);
    }

    transform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType>(
        ::fwData::TransformationMatrix3D::s_MODIFIED_SIG)
    ->asyncEmit();
}

//------------------------------------------------------------------------------

void SFastRegistration::computeRegistration(::fwCore::HiResClock::HiResClockType)
{
    this->updating();
}

} // namespace opItkRegistration

