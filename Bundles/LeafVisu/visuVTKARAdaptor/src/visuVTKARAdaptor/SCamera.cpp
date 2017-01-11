/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkPerspectiveTransform.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKARAdaptor::SCamera,
                         ::fwData::TransformationMatrix3D );

namespace visuVTKARAdaptor
{
//------------------------------------------------------------------------------

class CameraCallback : public ::vtkCommand
{
public:

    static CameraCallback* New(::visuVTKARAdaptor::SCamera* adaptor)
    {
        CameraCallback* cb = new CameraCallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    CameraCallback()
    {
        m_adaptor = NULL;
    }
    ~CameraCallback()
    {
    }

    virtual void Execute(::vtkObject* pCaller, unsigned long eventId, void*)
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKARAdaptor::SCamera* m_adaptor;
};

static const double s_nearPlane = 0.1;
static const double s_farPlane  = 10000;

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SCamera::s_POSITION_MODIFIED_SIG = "positionModified";

const ::fwCom::Slots::SlotKeyType SCamera::s_CALIBRATE_SLOT = "calibrate";

//------------------------------------------------------------------------------

SCamera::SCamera() throw() :
    m_transOrig(nullptr),
    m_cameraCommand(CameraCallback::New(this))
{
    newSignal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    m_slotCalibrate = newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() throw()
{
}

//------------------------------------------------------------------------------

void SCamera::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    m_cameraUID = m_configuration->getAttributeValue("cameraUID");
}

//------------------------------------------------------------------------------

void SCamera::doStart() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();

    double position[] = { 0.0, 0.0, 0.0 };
    double focal[]    = { 0.0, 0.0, 1.0 };
    double viewUp[]   = { 0.0, -1.0, 0.0 };

    m_transOrig = vtkPerspectiveTransform::New();
    m_transOrig->Identity();
    m_transOrig->SetupCamera(position, focal, viewUp);

    camera->SetPosition(position);
    camera->SetFocalPoint(focal);
    camera->SetViewUp(viewUp);
    camera->SetClippingRange(s_nearPlane, s_farPlane);

    camera->AddObserver(::vtkCommand::ModifiedEvent, m_cameraCommand);

    if (!m_cameraUID.empty())
    {
        m_camera = this->getSafeInput< ::arData::Camera>(m_cameraUID);
        SLM_ASSERT("Missing camera", m_camera);

        m_connections.connect(m_camera, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);

        this->calibrate();
    }
    this->updateFromTMatrix3D();
}

//------------------------------------------------------------------------------

void SCamera::doUpdate() throw(fwTools::Failed)
{
    this->updateFromTMatrix3D();
}

//------------------------------------------------------------------------------

void SCamera::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void SCamera::doStop() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);
    m_transOrig->Delete();
    m_connections.disconnect();
}

//-----------------------------------------------------------------------------

void SCamera::updateFromVtk()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();

    vtkPerspectiveTransform* trans = vtkPerspectiveTransform::New();
    trans->Identity();
    trans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());
    this->calibrate();

    trans->Inverse();
    trans->Concatenate(m_transOrig);
    vtkMatrix4x4* mat = trans->GetMatrix();

    for (int lt = 0; lt < 4; lt++)
    {
        for (int ct = 0; ct < 4; ct++)
        {
            trf->setCoefficient(lt, ct, mat->GetElement(lt, ct));
        }
    }

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    camera->AddObserver(::vtkCommand::ModifiedEvent, m_cameraCommand);

    trans->Delete();
}


//-----------------------------------------------------------------------------

void SCamera::updateFromTMatrix3D()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);

    ::fwData::TransformationMatrix3D::sptr transMat = this->getObject< ::fwData::TransformationMatrix3D >();

    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    for (int lt = 0; lt < 4; lt++)
    {
        for (int ct = 0; ct < 4; ct++)
        {
            mat->SetElement(lt, ct, transMat->getCoefficient(lt, ct));
        }
    }

    // Position camera on origin
    vtkPerspectiveTransform* oldTrans = vtkPerspectiveTransform::New();
    oldTrans->Identity();
    oldTrans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());
    oldTrans->Inverse();
    oldTrans->Concatenate(m_transOrig);
    oldTrans->Inverse();

    // Apply new transform
    vtkTransform* trans = vtkTransform::New();
    trans->SetMatrix(mat);
    trans->Concatenate(oldTrans->GetMatrix());
    camera->ApplyTransform(trans);

    this->setVtkPipelineModified();

    // Reset the clipping range as well since vtk interactor modifies it
    camera->SetClippingRange(s_nearPlane, s_farPlane);
    camera->AddObserver(::vtkCommand::ModifiedEvent, m_cameraCommand);

    mat->Delete();
    oldTrans->Delete();
    trans->Delete();

    this->requestRender();

    // notify that the camera position is modified
    auto sig = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SCamera::calibrate()
{
    if ( m_camera )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();
        camera->RemoveObserver(m_cameraCommand);

        double fy = m_camera->getFy();
        camera->SetViewAngle(2.0 * atan(m_camera->getHeight() / 2.0 / fy) * 180./ vtkMath::Pi());

        this->updateFromTMatrix3D();
        camera->AddObserver(::vtkCommand::ModifiedEvent, m_cameraCommand);
        this->setVtkPipelineModified();
    }
}

} //namespace visuVTKARAdaptor
