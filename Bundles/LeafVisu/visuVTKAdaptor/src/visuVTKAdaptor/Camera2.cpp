/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>


#include <fwData/Boolean.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkIdentityTransform.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkPerspectiveTransform.h>

#include "visuVTKAdaptor/Camera2.hpp"

class Camera2Clallback : public ::vtkCommand
{
public:

    static Camera2Clallback* New(::visuVTKAdaptor::Camera2* adaptor) {
        Camera2Clallback *cb = new Camera2Clallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

     Camera2Clallback() {}
    ~Camera2Clallback() {}

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::Camera2 *m_adaptor;
};

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Camera2, ::fwData::TransformationMatrix3D ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Camera2::Camera2() throw()
{
    m_cameraCommand = Camera2Clallback::New(this);
}

//------------------------------------------------------------------------------

Camera2::~Camera2() throw()
{}

//------------------------------------------------------------------------------

void Camera2::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void Camera2::doStart() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();

    double position[]={0.0, 0.0, 0.0};
    double focal[]={1.0, 0.0, 0.0};
    double viewUp[]={0.0, 0.0, 1.0};

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

void Camera2::doUpdate() throw(fwTools::Failed)
{}

//------------------------------------------------------------------------------

void Camera2::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Camera2::doStop() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );
    this->unregisterServices();
    m_transOrig->Delete();
}

//------------------------------------------------------------------------------

void Camera2::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if( msg->hasEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();
        camera->RemoveObserver( m_cameraCommand );

        ::fwData::TransformationMatrix3D::sptr transMat = this->getObject< ::fwData::TransformationMatrix3D >();

        vtkMatrix4x4* mat = vtkMatrix4x4::New();

        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
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
        vtkTransform* trans =  vtkTransform::New();
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
}

//------------------------------------------------------------------------------

void Camera2::updateFromVtk()
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->RemoveObserver( m_cameraCommand );

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();

    vtkPerspectiveTransform* trans = vtkPerspectiveTransform::New();
    trans->Identity();
    trans->SetupCamera(camera->GetPosition(), camera->GetFocalPoint(), camera->GetViewUp());

    trans->Inverse();
    trans->Concatenate(m_transOrig);
    vtkMatrix4x4* mat = trans->GetMatrix();

    for(int lt=0; lt<4; lt++)
    {
        for(int ct=0; ct<4; ct++)
        {
            trf->setCoefficient(lt,ct, mat->GetElement(lt,ct));
        }
    }

    ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), trf, msg);

    camera->AddObserver( ::vtkCommand::ModifiedEvent, m_cameraCommand );

    trans->Delete();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
