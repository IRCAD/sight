/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/STransformFromWheel.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vtkAbstractPropPicker.h>

namespace visuVTKAdaptor
{

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::STransformFromWheel);

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TRANSFORM_SLOT  = "updateTransform";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT     = "image";
static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

STransformFromWheel::STransformFromWheel() :
    m_initAngle(0.)
{
    newSlot(s_UPDATE_TRANSFORM_SLOT, &STransformFromWheel::updateTransform, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &STransformFromWheel::updateSliceOrientation, this);
}

//------------------------------------------------------------------------------

STransformFromWheel::~STransformFromWheel()
{

}

//------------------------------------------------------------------------------

void STransformFromWheel::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void STransformFromWheel::starting()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void STransformFromWheel::updating()
{

}

//------------------------------------------------------------------------------

void STransformFromWheel::stopping()
{

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STransformFromWheel::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void STransformFromWheel::updateSliceOrientation(int from, int to)
{
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation = static_cast< Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< Orientation >( to );
    }
}

//------------------------------------------------------------------------------

void STransformFromWheel::updateTransform(double cx, double cy, double wheelAngle)
{
    vtkAbstractPropPicker* picker = this->getRenderService()->getPicker(m_pickerId);

    const double angle = wheelAngle - m_initAngle;
    m_initAngle = wheelAngle;

    SLM_ASSERT("No picker named '" + m_pickerId + "'.", picker);

    double displayPos[3] = { cx, cy, 0. };
    picker->Pick( displayPos, this->getRenderer());

    double worldPos[3];
    ::fwRenderVTK::vtk::getNearestPickedPosition(picker, this->getRenderer(), worldPos);

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    SLM_ASSERT("No 'image' found.", image);

    m_weakImage = image;

    int imageIndex[3];
    this->worldToSliceIndex(worldPos, imageIndex);

    const auto& spacing = image->getSpacing();

    ::glm::dvec3 rotAxis;
    ::glm::dvec3 pos;

    switch (m_orientation)
    {
        case Z_AXIS:
            rotAxis = ::glm::dvec3(0., 0., 1.);
            pos     = ::glm::dvec3(imageIndex[0] * spacing[0], imageIndex[1] * spacing[1], imageIndex[2] * spacing[2]);
            break;
        case Y_AXIS:
            rotAxis = ::glm::dvec3(0., -1., 0.);
            pos     = ::glm::dvec3(imageIndex[0] * spacing[0], imageIndex[2] * spacing[2], imageIndex[1] * spacing[1]);
            break;
        case X_AXIS:
            rotAxis = ::glm::dvec3(1., 0., 0.);
            pos     = ::glm::dvec3(imageIndex[2] * spacing[2], imageIndex[0] * spacing[0], imageIndex[1] * spacing[1]);
            break;
    }

    const ::glm::dmat4 invTransMat = ::glm::translate(pos);
    const ::glm::dmat4 rotMat      = ::glm::rotate(invTransMat, angle, rotAxis);
    const ::glm::dmat4 centeredRot = ::glm::translate(rotMat, -pos);

    ::fwData::TransformationMatrix3D::sptr outTrans =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    SLM_ASSERT("No 'transform' found.", outTrans);

    ::glm::dmat4 finalTransform = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(outTrans);
    finalTransform              = finalTransform * centeredRot;

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(outTrans, finalTransform);

    auto sig = outTrans->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
