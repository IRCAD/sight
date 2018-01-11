/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SCamera.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

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

class CameraClallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static CameraClallback* New(::visuVTKAdaptor::SCamera* adaptor)
    {
        CameraClallback* cb = new CameraClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    CameraClallback() :
        m_adaptor(nullptr)
    {
    }
    ~CameraClallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::SCamera* m_adaptor;
};

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SCamera);

static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept :
    m_cameraCommand(CameraClallback::New(this)),
    m_transOrig(nullptr)
{
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->initialize();

    vtkCamera* camera = this->getRenderer()->GetActiveCamera();

    double position[] = {0.0, 0.0, 0.0};
    double focal[]    = {1.0, 0.0, 0.0};
    double viewUp[]   = {0.0, 0.0, 1.0};

    m_transOrig = vtkPerspectiveTransform::New();
    m_transOrig->Identity();
    m_transOrig->SetupCamera(position, focal, viewUp );

    camera->SetPosition(position);
    camera->SetFocalPoint(focal);
    camera->SetViewUp(viewUp);
    //camera->SetClippingRange(0.1, 10000);

    camera->AddObserver( ::vtkCommand::ModifiedEvent, m_cameraCommand );
    this->requestRender();
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );

    ::fwData::TransformationMatrix3D::csptr transMat =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    ::fwData::mt::ObjectReadLock lock(transMat);

    for(int lt = 0; lt < 4; lt++)
    {
        for(int ct = 0; ct < 4; ct++)
        {
            mat->SetElement(lt, ct, transMat->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
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
    this->requestRender();
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );
    this->unregisterServices();
    m_transOrig->Delete();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SCamera::updateFromVtk()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );

    ::fwData::TransformationMatrix3D::sptr trf = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);
    ::fwData::mt::ObjectWriteLock lock(trf);

    vtkPerspectiveTransform* trans = vtkPerspectiveTransform::New();
    trans->Identity();
    trans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());

    trans->Inverse();
    trans->Concatenate(m_transOrig);
    vtkMatrix4x4* mat = trans->GetMatrix();

    for(int lt = 0; lt < 4; lt++)
    {
        for(int ct = 0; ct < 4; ct++)
        {
            trf->setCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct), mat->GetElement(lt, ct));
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

::fwServices::IService::KeyConnectionsMap SCamera::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
