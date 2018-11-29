/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "opItkRegistration/SResampler.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <itkRegistrationOp/Resampler.hpp>

namespace opItkRegistration
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opItkRegistration::SResampler, ::fwData::Image);

static const ::fwServices::IService::KeyType s_IMAGE_IN    = "imageIn";
static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "imageOut";

static const ::fwServices::IService::KeyType s_TARGET_IN    = "target";
static const ::fwServices::IService::KeyType s_TRANSFORM_IN = "transform";

//------------------------------------------------------------------------------

SResampler::SResampler()
{

}

//------------------------------------------------------------------------------

SResampler::~SResampler()
{

}

//------------------------------------------------------------------------------

void SResampler::configuring()
{

}

//------------------------------------------------------------------------------

void SResampler::starting()
{

}

//------------------------------------------------------------------------------

void SResampler::updating()
{
    ::fwData::Image::csptr inImg = this->getInput< ::fwData::Image >(s_IMAGE_IN);

    ::fwData::mt::ObjectReadLock inImLock(inImg);

    ::fwData::Image::sptr outImg = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    ::fwData::mt::ObjectWriteLock outImLock(outImg);

    ::fwData::Image::csptr target = this->getInput< ::fwData::Image >(s_TARGET_IN);

    ::fwData::mt::ObjectReadLock targetLock(target);

    ::fwData::TransformationMatrix3D::csptr transform =
        this->getInput< ::fwData::TransformationMatrix3D >(s_TRANSFORM_IN);

    SLM_ASSERT("No 'imageIn' found !", inImg);
    SLM_ASSERT("No 'imageOut' found !", outImg);
    SLM_ASSERT("No 'transform' found !", transform);

    ::itkRegistrationOp::Resampler::resample(inImg, outImg, transform, target);

    m_sigComputed->asyncEmit();

    auto imgBufModifSig = outImg->signal< ::fwData::Image::BufferModifiedSignalType >
                              (::fwData::Image::s_BUFFER_MODIFIED_SIG);

    imgBufModifSig->asyncEmit();

    auto imgModifSig = outImg->signal< ::fwData::Image::ModifiedSignalType >
                           (::fwData::Image::s_MODIFIED_SIG);

    imgModifSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SResampler::stopping()
{

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SResampler::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TRANSFORM_IN, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TARGET_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace opItkRegistration
