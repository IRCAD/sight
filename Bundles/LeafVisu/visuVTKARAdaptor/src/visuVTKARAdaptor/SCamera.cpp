/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkPerspectiveTransform.h>
#include <vtkTransform.h>
#include <vtkMath.h>

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
        CameraCallback *cb = new CameraCallback;
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

    ::visuVTKARAdaptor::SCamera *m_adaptor;
};

static const double s_nearPlane = 0.1;
static const double s_farPlane  = 10000;

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SCamera::s_CALIBRATE_SLOT = "calibrate";

//------------------------------------------------------------------------------

SCamera::SCamera() throw()
{
    m_cameraCommand = CameraCallback::New(this);

    m_slotCalibrate = ::fwCom::newSlot( &SCamera::calibrate, this);

    ::fwCom::HasSlots::m_slots(s_CALIBRATE_SLOT, m_slotCalibrate);



    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() throw()
{
}

//------------------------------------------------------------------------------

void SCamera::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

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

    m_connections = ::fwServices::helper::SigSlotConnection::New();

    if (!m_cameraUID.empty())
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_cameraUID);

        m_camera = ::arData::Camera::dynamicCast(obj);
        SLM_ASSERT("Missing camera", m_camera);

        m_connections->connect(m_camera, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG,
                               this->getSptr(), s_CALIBRATE_SLOT);

        this->calibrate();
    }
    this->updateFromTMatrix3D();
}

//------------------------------------------------------------------------------

void SCamera::doUpdate() throw(fwTools::Failed)
{

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
    m_connections->disconnect();
}

//------------------------------------------------------------------------------

void SCamera::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if (msg->hasEvent(::fwComEd::ImageMsg::MODIFIED))
    {
        this->calibrate();
    }

    if (msg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED))
    {
        this->updateFromTMatrix3D();
    }
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

    ::fwComEd::TransformationMatrix3DMsg::sptr msg = ::fwComEd::TransformationMatrix3DMsg::New();
    msg->addEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);
    msg->setSource(this->getSptr());
    msg->setSubject( trf);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = trf->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
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
