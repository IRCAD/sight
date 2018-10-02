/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/STransformFromWheel.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include <vtkAbstractPropPicker.h>

namespace visuVTKAdaptor
{

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::STransformFromWheel);

static const ::fwCom::Slots::SlotKeyType s_ROTATE_TRANSFORM_SLOT  = "rotateTransform";
static const ::fwCom::Slots::SlotKeyType s_TRANSLATE_TRANSFORM    = "translateTransform";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";

static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

STransformFromWheel::STransformFromWheel() :
    m_orientation(::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS),
    m_interactionMode(2),
    m_initAngle(0.),
    m_translate(false)
{
    newSlot(s_ROTATE_TRANSFORM_SLOT, &STransformFromWheel::rotateTransform, this);
    newSlot(s_TRANSLATE_TRANSFORM, &STransformFromWheel::translateTransform, this);
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

    const ConfigType config           = this->getConfigTree().get_child("config.<xmlattr>");
    const std::string interactionMode = config.get<std::string>("mode", "2D");

    if(interactionMode == "2d" || interactionMode == "2D")
    {
        m_interactionMode = 2;
    }
    else if(interactionMode == "3d" || interactionMode == "3D")
    {
        m_interactionMode = 3;
    }
    else
    {
        SLM_WARN("Wrong interaction mode specified. Set to 2D.");
    }

    const std::string orientation = config.get<std::string>("orientation", "");

    if(orientation == "axial")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS;
    }
    else
    {
        SLM_FATAL("Unknown orientation: '" + orientation + "'.");
    }
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

void STransformFromWheel::applyTransformToOutput(const glm::dmat4& transform) const
{
    ::fwData::TransformationMatrix3D::sptr outTrans =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    SLM_ASSERT("No 'transform' found.", outTrans);

    ::glm::dmat4 finalTransform = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(outTrans);
    finalTransform              = finalTransform * transform;

    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(outTrans, finalTransform);

    auto sig = outTrans->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >
                   (::fwData::TransformationMatrix3D::s_MODIFIED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void STransformFromWheel::updateSliceOrientation(int from, int to)
{
    if( to == static_cast< int > ( m_orientation ) )
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImage::Orientation > ( from );
    }
    else if(from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast< ::fwDataTools::helper::MedicalImage::Orientation >( to );
    }
}

//------------------------------------------------------------------------------

void STransformFromWheel::rotateTransform(double cx, double cy, double wheelAngle)
{
    const double angle = wheelAngle - m_initAngle;
    m_initAngle = wheelAngle;

    // Get wheel position in image space.
    vtkAbstractPropPicker* picker = this->getRenderService()->getPicker(m_pickerId);

    SLM_ASSERT("No picker named '" + m_pickerId + "'.", picker);

    double displayPosition[3] = { cx, cy, 0. };
    picker->Pick( displayPosition, this->getRenderer());

    double worldPos[3];
    ::fwRenderVTK::vtk::getNearestPickedPosition(picker, this->getRenderer(), worldPos);

    ::glm::dvec3 rotAxis;
    ::glm::dvec3 pos(worldPos[0], worldPos[1], worldPos[2]);

    rotAxis[m_orientation] = 1.;

    if(m_interactionMode == 2)
    {
        switch (m_orientation)
        {
            case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS: break;
            case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
                rotAxis = -rotAxis;
                pos     = ::glm::dvec3(pos.x, pos.z, pos.y);
                break;
            case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
                pos = ::glm::dvec3(pos.z, pos.x, pos.y);
                break;
        }
    }

    const ::glm::dmat4 invTransMat = ::glm::translate(::glm::dmat4(1.), pos);
    const ::glm::dmat4 rotMat      = ::glm::rotate(invTransMat, angle, rotAxis);
    const ::glm::dmat4 centeredRot = ::glm::translate(rotMat, -pos);

    this->applyTransformToOutput(centeredRot);
}

//------------------------------------------------------------------------------

void STransformFromWheel::translateTransform(fwDataTools::PickingInfo info)
{
    if(info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN)
    {
        std::copy(info.m_worldPos, info.m_worldPos + 3, m_lastPickedPos);

        m_translate = true;
    }
    else if(m_translate && info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE)
    {
        ::glm::dvec3 transVec(m_lastPickedPos[0] - info.m_worldPos[0],
                              m_lastPickedPos[1] - info.m_worldPos[1],
                              m_lastPickedPos[2] - info.m_worldPos[2]);

        if(m_interactionMode == 2)
        {
            switch (m_orientation)
            {
                case ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS: break;
                case ::fwDataTools::helper::MedicalImage::Orientation::Y_AXIS:
                    transVec = ::glm::dvec3(transVec.x, transVec.z, transVec.y);
                    break;
                case ::fwDataTools::helper::MedicalImage::Orientation::X_AXIS:
                    transVec = ::glm::dvec3(transVec.z, transVec.x, transVec.y);
                    break;
            }
        }

        const ::glm::dmat4 transMat = ::glm::translate(::glm::dmat4(1.), transVec);

        this->applyTransformToOutput(transMat);

        std::copy(info.m_worldPos, info.m_worldPos + 3, m_lastPickedPos);
    }
    else if(info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP)
    {
        m_translate = false;
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
