/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opItkRegistration/SImageCenter.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <itkRegistrationOp/AutomaticRegistration.hpp>

namespace opItkRegistration
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opItkRegistration::SImageCenter);

static const ::fwServices::IService::KeyType s_IMAGE_IN        = "image";
static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

SImageCenter::SImageCenter()
{

}

//------------------------------------------------------------------------------

SImageCenter::~SImageCenter()
{

}

//------------------------------------------------------------------------------

void SImageCenter::configuring() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SImageCenter::starting() throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

void SImageCenter::updating() throw( ::fwTools::Failed )
{

    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    ::fwData::mt::ObjectReadLock imLock(image);

    SLM_ASSERT("Missing image '"+ s_IMAGE_IN + "'", image);

    const bool imageValidity = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);

    if(!imageValidity)
    {
        SLM_WARN("Can not compute center of a invalid image.");
        return;
    }

    ::fwData::TransformationMatrix3D::sptr matrix =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    SLM_ASSERT("Missing matrix '"+ s_TRANSFORM_INOUT +"'", matrix);

    ::fwData::mt::ObjectWriteLock matLock(matrix);

    ::fwDataTools::TransformationMatrix3D::identity(matrix);

    //compute the center
    const ::fwData::Image::SizeType size       = image->getSize();
    const ::fwData::Image::SpacingType spacing = image->getSpacing();
    const ::fwData::Image::OriginType origin   = image->getOrigin();

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

    auto sig = matrix->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    sig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//------------------------------------------------------------------------------

void SImageCenter::stopping() throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageCenter::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace opItkRegistration
