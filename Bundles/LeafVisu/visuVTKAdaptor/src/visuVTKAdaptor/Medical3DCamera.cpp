/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Medical3DCamera.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <boost/assign/list_of.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::Medical3DCamera, ::fwData::Object );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType SET_AXIAL_SLOT    = "setAxial";
static const ::fwCom::Slots::SlotKeyType SET_SAGITTAL_SLOT = "setSagittal";
static const ::fwCom::Slots::SlotKeyType SET_FRONTAL_SLOT  = "setFrontal";

std::map< std::string, ::fwDataTools::helper::MedicalImageAdaptor::Orientation >
Medical3DCamera::m_orientationConversion = ::boost::assign::map_list_of
                                               (std::string("axial"), Z_AXIS)
                                               (std::string("frontal"), Y_AXIS)
                                               (std::string("sagittal"), X_AXIS);

//------------------------------------------------------------------------------

Medical3DCamera::Medical3DCamera() noexcept :
    m_resetAtStart(false)

{
    m_slotSetAxial    = ::fwCom::newSlot(&Medical3DCamera::setAxialView, this);
    m_slotSetSagittal = ::fwCom::newSlot(&Medical3DCamera::setSagittalView, this);
    m_slotSetFrontal  = ::fwCom::newSlot(&Medical3DCamera::setFrontalView, this);

    ::fwCom::HasSlots::m_slots(SET_AXIAL_SLOT, m_slotSetAxial)
        (SET_SAGITTAL_SLOT, m_slotSetSagittal)
        (SET_FRONTAL_SLOT, m_slotSetFrontal);

    this->setWorker(m_associatedWorker);
}

//------------------------------------------------------------------------------

Medical3DCamera::~Medical3DCamera() noexcept
{
}

//------------------------------------------------------------------------------

void Medical3DCamera::doConfigure()
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");
        SLM_ASSERT("Unknown orientation", m_orientationConversion.find(orientation) != m_orientationConversion.end());
        m_orientation = m_orientationConversion[orientation];
    }

    if(m_configuration->hasAttribute("resetAtStart"))
    {
        std::string reset = m_configuration->getAttributeValue("resetAtStart");
        SLM_ASSERT("'resetAtStart' value must be 'yes' or 'no'",
                   reset == "yes" || reset == "no");
        m_resetAtStart = (reset == "yes");
    }
}

//------------------------------------------------------------------------------

void Medical3DCamera::doStart()
{
    m_camera = this->getRenderer()->GetActiveCamera();

    if(m_resetAtStart)
    {
        this->updateView();
    }
}

//------------------------------------------------------------------------------

void Medical3DCamera::doUpdate()
{
    SLM_TRACE_FUNC();
    this->updateView();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doStop()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Medical3DCamera::setSagittalView()
{
    m_orientation = X_AXIS;
    this->updating();
}

//------------------------------------------------------------------------------

void Medical3DCamera::setFrontalView()
{
    m_orientation = Y_AXIS;
    this->updating();
}

//------------------------------------------------------------------------------

void Medical3DCamera::setAxialView()
{
    m_orientation = Z_AXIS;
    this->updating();
}

//------------------------------------------------------------------------------

void Medical3DCamera::updateView()
{
    if(m_orientation == Z_AXIS )
    {
        this->resetAxialView();
    }
    else if(m_orientation == Y_AXIS )
    {
        this->resetFrontalView();
    }
    else if(m_orientation == X_AXIS )
    {
        this->resetSagittalView();
    }
}

//------------------------------------------------------------------------------

void Medical3DCamera::resetSagittalView()
{
    m_camera->SetPosition(-1, 0, 0);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, 0, 1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Medical3DCamera::resetFrontalView()
{
    m_camera->SetPosition(0, -1, 0);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, 0, 1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------

void Medical3DCamera::resetAxialView()
{
    m_camera->SetPosition(0, 0, -1);
    m_camera->SetFocalPoint(0, 0, 0);
    m_camera->SetViewUp(0, -1, 0);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

