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

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TRANSFORM_SLOT = "updateTransform";

//------------------------------------------------------------------------------

STransformFromWheel::STransformFromWheel()
{
    newSlot(s_UPDATE_TRANSFORM_SLOT, &STransformFromWheel::updateTransform, this);
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

void STransformFromWheel::updateTransform(double cx, double cy, double o)
{
    vtkAbstractPropPicker* picker = this->getRenderService()->getPicker(m_pickerId);

    SLM_ASSERT("No picker named '" + m_pickerId + "'.", picker);

    double displayPos[3] = { cx, cy, 0. };
    picker->Pick( displayPos, this->getRenderer());

    double worldPos[3];
    ::fwRenderVTK::vtk::getNearestPickedPosition(picker, this->getRenderer(), worldPos);

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >("image");

    m_weakImage = image;

    int imageIndex[3];
    this->worldToSliceIndex(worldPos, imageIndex);

    std::cout << "Index " << imageIndex[0] << " " << imageIndex[1] << " "<< imageIndex[2] << " " << std::endl;

    ::glm::dvec3 rotAxis;
    switch (m_orientation)
    {
        case Z_AXIS:
            rotAxis = ::glm::dvec3(0., 0., 1.);
            break;
        case Y_AXIS:
            rotAxis = ::glm::dvec3(0., 1., 0.);
            break;
        case X_AXIS:
            rotAxis = ::glm::dvec3(1., 0., 0.);
            break;
    }

    const ::glm::dvec3 pos(imageIndex[0], imageIndex[1], imageIndex[2]);
    const ::glm::dmat4 invTransMat    = ::glm::translate(-pos);
    const ::glm::dmat4 rotMat         = ::glm::rotate(invTransMat, o, rotAxis);
    const ::glm::dmat4 finalTransform = ::glm::translate(rotMat, pos);

    ::fwData::TransformationMatrix3D::sptr outTrans = this->getInOut< ::fwData::TransformationMatrix3D >("transform");

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(outTrans, finalTransform);

    auto sig = outTrans->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
