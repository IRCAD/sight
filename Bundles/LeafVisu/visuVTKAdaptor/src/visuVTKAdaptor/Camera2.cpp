/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Camera2.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkIdentityTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

class Camera2Clallback : public ::vtkCommand
{
public:

    static Camera2Clallback* New(::visuVTKAdaptor::Camera2* adaptor)
    {
        Camera2Clallback* cb = new Camera2Clallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    Camera2Clallback() : m_adaptor(nullptr)
    {
    }
    ~Camera2Clallback()
    {
    }

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::Camera2* m_adaptor;
};

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Camera2,
                         ::fwData::TransformationMatrix3D );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Camera2::Camera2() noexcept :
    m_cameraCommand(Camera2Clallback::New(this)),
    m_transOrig(nullptr)
{
}

//------------------------------------------------------------------------------

Camera2::~Camera2() noexcept
{
}

//------------------------------------------------------------------------------

void Camera2::doConfigure()
{
}

//------------------------------------------------------------------------------

void Camera2::doStart()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();

    double position[] = {0.0, 0.0, 0.0};
    double focal[]    = {1.0, 0.0, 0.0};
    double viewUp[]   = {0.0, 0.0, 1.0};

    m_transOrig = vtkPerspectiveTransform::New();
    m_transOrig->Identity();
    m_transOrig->SetupCamera(position, focal, viewUp );

    camera->SetPosition (position);
    camera->SetFocalPoint(focal);
    camera->SetViewUp(viewUp);
    //camera->SetClippingRange(0.1, 10000);

    camera->AddObserver( ::vtkCommand::ModifiedEvent, m_cameraCommand );
}

//------------------------------------------------------------------------------

void Camera2::doUpdate()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );

    ::fwData::TransformationMatrix3D::sptr transMat = this->getObject< ::fwData::TransformationMatrix3D >();

    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    ::fwData::mt::ObjectReadLock lock(transMat);

    for(int lt = 0; lt<4; lt++)
    {
        for(int ct = 0; ct<4; ct++)
        {
            mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
        }
    }

    lock.unlock();

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

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();

    camera->AddObserver( ::vtkCommand::ModifiedEvent, m_cameraCommand );

    mat->Delete();
    oldTrans->Delete();
    trans->Delete();
}

//------------------------------------------------------------------------------

void Camera2::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Camera2::doStop()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );
    this->unregisterServices();
    m_transOrig->Delete();
}

//------------------------------------------------------------------------------

void Camera2::updateFromVtk()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    ::fwData::mt::ObjectWriteLock lock(trf);

    vtkPerspectiveTransform* trans = vtkPerspectiveTransform::New();
    trans->Identity();
    trans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());

    trans->Inverse();
    trans->Concatenate(m_transOrig);
    vtkMatrix4x4* mat = trans->GetMatrix();

    for(int lt = 0; lt<4; lt++)
    {
        for(int ct = 0; ct<4; ct++)
        {
            trf->setCoefficient(lt,ct, mat->GetElement(lt,ct));
        }
    }

    lock.unlock();

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    camera->AddObserver( ::vtkCommand::ModifiedEvent, m_cameraCommand );

    trans->Delete();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
