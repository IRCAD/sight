/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <itkRegistrationOp/Resampler.hpp>

#include <services/macros.hpp>

namespace opItkRegistration
{

fwServicesRegisterMacro( ::sight::services::IOperator, ::opItkRegistration::SResampler, data::Image)

static const services::IService::KeyType s_IMAGE_IN = "imageIn";
static const services::IService::KeyType s_IMAGE_INOUT = "imageOut";

static const services::IService::KeyType s_TARGET_IN    = "target";
static const services::IService::KeyType s_TRANSFORM_IN = "transform";

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
    data::Image::csptr inImg = this->getInput< data::Image >(s_IMAGE_IN);

    data::mt::ObjectReadLock inImLock(inImg);

    data::Image::sptr outImg = this->getInOut< data::Image >(s_IMAGE_INOUT);

    data::mt::ObjectWriteLock outImLock(outImg);

    data::Image::csptr target = this->getInput< data::Image >(s_TARGET_IN);

    data::mt::ObjectReadLock targetLock(target);

    data::TransformationMatrix3D::csptr transform =
        this->getInput< data::TransformationMatrix3D >(s_TRANSFORM_IN);

    SLM_ASSERT("No 'imageIn' found !", inImg);
    SLM_ASSERT("No 'imageOut' found !", outImg);
    SLM_ASSERT("No 'transform' found !", transform);

    ::itkRegistrationOp::Resampler::resample(inImg, outImg, transform, target);

    m_sigComputed->asyncEmit();

    auto imgBufModifSig = outImg->signal< data::Image::BufferModifiedSignalType >
                              (data::Image::s_BUFFER_MODIFIED_SIG);

    imgBufModifSig->asyncEmit();

    auto imgModifSig = outImg->signal< data::Image::ModifiedSignalType >
                           (data::Image::s_MODIFIED_SIG);

    imgModifSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SResampler::stopping()
{

}

//------------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SResampler::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TRANSFORM_IN, data::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TARGET_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace opItkRegistration
