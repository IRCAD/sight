/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkPerspectiveTransform.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SCamera,
                         ::fwData::TransformationMatrix3D );

namespace visuVTKARAdaptor
{

static const ::fwServices::IService::KeyType s_CAMERA_IN       = "camera";
static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

class WindowResizeCallBack : public ::vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static WindowResizeCallBack* New(::visuVTKARAdaptor::SCamera* adaptor)
    {
        WindowResizeCallBack* cb = new WindowResizeCallBack;
        cb->m_adaptor = adaptor;
        return cb;
    }

    WindowResizeCallBack()
    {
        m_adaptor = NULL;
    }
    ~WindowResizeCallBack()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute(::vtkObject* pCaller, unsigned long eventId, void*)
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKARAdaptor::SCamera* m_adaptor;
};

//------------------------------------------------------------------------------

class CameraCallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------

    virtual void Execute(::vtkObject* pCaller, unsigned long eventId, void*)
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKARAdaptor::SCamera* m_adaptor;
};

static const double s_nearPlane = 1.;
static const double s_farPlane  = 10000;

//------------------------------------------------------------------------------

typedef ::fwCom::Signal<void ()> PositionModifiedSignalType;

static const ::fwCom::Signals::SignalKeyType s_POSITION_MODIFIED_SIG = "positionModified";

static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT = "calibrate";

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept :
    m_transOrig(nullptr),
    m_cameraCommand(CameraCallback::New(this)),
    m_resizeCommand(WindowResizeCallBack::New(this))
{
    newSignal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
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

    this->calibrate();

    this->updateFromTMatrix3D();

    camera->AddObserver(::vtkCommand::ModifiedEvent, m_cameraCommand);
    this->getRenderer()->AddObserver(::vtkCommand::ModifiedEvent, m_resizeCommand);
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
    this->updateFromTMatrix3D();
}

//------------------------------------------------------------------------------

void SCamera::swapping()
{
    this->stopping();
    this->starting();
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);
    m_transOrig->Delete();
}

//-----------------------------------------------------------------------------

void SCamera::updateFromVtk()
{

    if(this->isStopped())
    {
        return;
    }

    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);

    ::fwData::TransformationMatrix3D::sptr trf = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    vtkPerspectiveTransform* trans = vtkPerspectiveTransform::New();
    trans->Identity();
    trans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());

    this->calibrate();

    trans->Inverse();
    trans->Concatenate(m_transOrig);
    vtkMatrix4x4* mat = trans->GetMatrix();

    for (std::uint8_t lt = 0; lt < 4; lt++)
    {
        for (std::uint8_t ct = 0; ct < 4; ct++)
        {
            trf->setCoefficient(lt, ct, mat->GetElement(lt, ct));
        }
    }

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    trans->Delete();
}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SCamera::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_CAMERA_IN, ::arData::Camera::s_MODIFIED_SIG, s_CALIBRATE_SLOT);
    connections.push(s_CAMERA_IN, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT);
    connections.push(s_TRANSFORM_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SCamera::updateFromTMatrix3D()
{
    if(this->isStopped())
    {
        return;
    }

    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver(m_cameraCommand);

    ::fwData::TransformationMatrix3D::sptr transMat =
        this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    for (std::uint8_t lt = 0; lt < 4; lt++)
    {
        for (std::uint8_t ct = 0; ct < 4; ct++)
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
    ::arData::Camera::csptr cameraCalibration = this->getInput< ::arData::Camera >(s_CAMERA_IN);

    if ( cameraCalibration )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();
        camera->RemoveObserver(m_cameraCommand);

        const double fy = cameraCalibration->getFy();

        const double cx = cameraCalibration->getCx();
        const double cy = cameraCalibration->getCy();

        const double nx = static_cast<double>(cameraCalibration->getWidth());
        const double ny = static_cast<double>(cameraCalibration->getHeight());

        const double wcx = -2. * ( cx - nx / 2. ) / nx;
        const double wcy = 2. * ( cy - ny / 2. ) / ny;
        camera->SetViewAngle(2.0 * atan(cameraCalibration->getHeight() / 2.0 / fy) * 180./ vtkMath::Pi());

        // Use the Renderer aspect ratio when shifting window center to avoid bug when resizing windows.
        camera->SetWindowCenter(wcx / this->getRenderer()->GetAspect()[0], wcy);

        this->updateFromTMatrix3D();
        this->setVtkPipelineModified();
    }
}

} //namespace visuVTKARAdaptor
