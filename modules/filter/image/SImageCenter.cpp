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

#include "SImageCenter.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>

#include <service/macros.hpp>

#include <filter/image/AutomaticRegistration.hpp>

#include <geometry/data/Matrix4.hpp>

namespace sight::modules::filter::image
{

fwServicesRegisterMacro( ::sight::service::IOperator, ::sight::modules::filter::image::SImageCenter)

static const service::IService::KeyType s_IMAGE_IN = "image";
static const service::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

SImageCenter::SImageCenter()
{

}

//------------------------------------------------------------------------------

SImageCenter::~SImageCenter()
{

}

//------------------------------------------------------------------------------

void SImageCenter::configuring()
{
}

//------------------------------------------------------------------------------

void SImageCenter::starting()
{

}

//------------------------------------------------------------------------------

void SImageCenter::updating()
{

    data::Image::csptr image = this->getInput< data::Image >(s_IMAGE_IN);
    data::mt::ObjectReadLock imLock(image);

    SLM_ASSERT("Missing image '"+ s_IMAGE_IN + "'", image);

    const bool imageValidity = data::tools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);

    if(!imageValidity)
    {
        SLM_WARN("Can not compute center of a invalid image.");
        return;
    }

    data::Matrix4::sptr matrix =
        this->getInOut< data::Matrix4 >(s_TRANSFORM_INOUT);

    SLM_ASSERT("Missing matrix '"+ s_TRANSFORM_INOUT +"'", matrix);

    data::mt::ObjectWriteLock matLock(matrix);

    geometry::data::identity(matrix);

    //compute the center
    const data::Image::Size size       = image->getSize2();
    const data::Image::Spacing spacing = image->getSpacing2();
    const data::Image::Origin origin   = image->getOrigin2();

    SLM_ASSERT("Image should be in 3 Dimensions", size.size() == 3);

    std::vector<double> center(3, 0.);

    center[0] = (static_cast<double>(size[0]) * spacing[0]) / 2.;
    center[1] = (static_cast<double>(size[1]) * spacing[1]) / 2.;
    center[2] = (static_cast<double>(size[2]) * spacing[2]) / 2.;

    //compute origin -center

    center[0] += origin[0];
    center[1] += origin[1];
    center[2] += origin[2];

    matrix->setCoefficient(0, 3, center[0]);
    matrix->setCoefficient(1, 3, center[1]);
    matrix->setCoefficient(2, 3, center[2]);

    // output the translation matrix

    auto sig = matrix->signal< data::Matrix4::ModifiedSignalType >
                   (data::Matrix4::s_MODIFIED_SIG);

    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//------------------------------------------------------------------------------

void SImageCenter::stopping()
{

}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SImageCenter::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::modules::filter::image
